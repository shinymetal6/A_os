/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * space_echo.c
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "space_echo.h"

static float read_delay_line(float *buffer, uint32_t size, uint32_t wptr, float delay_samples) {
    float rptr_f = (float)wptr - delay_samples;
    if (rptr_f < 0) rptr_f += (float)size;

    uint32_t rptr_i = (uint32_t)rptr_f;
    float frac = rptr_f - (float)rptr_i;

    uint32_t rptr_next = (rptr_i + 1) % size;

    // Linear interpolation
    return buffer[rptr_i] + frac * (buffer[rptr_next] - buffer[rptr_i]);
}

ITCM_AREA_CODE static q15_t space_echo_process(SPACE_ECHO_Effect_TypeDef *echo, float input)
{
    const float fs = echo->sample_rate;

    // --- LFO for delay modulation (tape wow) ---
    echo->lfo_phase += 2.0f * M_PI * echo->f_lfo_rate / fs;
    if (echo->lfo_phase >= 2.0f * M_PI)
    	echo->lfo_phase -= 2.0f * M_PI;

    float lfo_val = sinf(echo->lfo_phase); // [-1, 1]
    float mod_delay_ms = echo->f_delay_time_ms + lfo_val * echo->f_lfo_depth_ms;
    if (mod_delay_ms < 20.0f)
    	mod_delay_ms = 20.0f;

    float delay_samples = mod_delay_ms * fs / 1000.0f;

    // --- Read delayed signal ---
    float delay_out = read_delay_line(echo->buffer, ECHO_BUFFER_SIZE, echo->write_ptr, delay_samples);

    // --- Apply low-pass filter to feedback path (simulate tape HF loss) ---
    float g = echo->g;
    float k = echo->k;
    float u = (delay_out - k * echo->y4) / (1.0f + g * (1.0f + g * (1.0f + g * (1.0f + g))));
    float stage = tanhf(u + g * echo->y1); echo->y1 = stage;
    stage = tanhf(stage + g * echo->y2);   echo->y2 = stage;
    stage = tanhf(stage + g * echo->y3);   echo->y3 = stage;
    stage = tanhf(stage + g * echo->y4);   echo->y4 = stage;
    float filtered_feedback = echo->y4;

    // --- Write input + feedback into delay line ---
    float write_val = input + filtered_feedback * echo->f_feedback;
    echo->buffer[echo->write_ptr] = write_val;

    // --- Advance pointer ---
    echo->write_ptr = (echo->write_ptr + 1) % ECHO_BUFFER_SIZE;

    // --- Output = dry + wet (adjust mix as needed) ---
    return __FLOAT_2_Q15(input + delay_out * 0.7f); // 70% wet
}

ITCM_AREA_CODE static void Effect_Space_Echo_Set_params(SPACE_ECHO_Effect_TypeDef *echo)
{
	if ( *echo->delay_time_ms == 0 )
		*echo->delay_time_ms = (uint16_t )ECHO_DEFAULT_DELAY_TIME;
	if (*echo->delay_time_ms < 20)
		*echo->delay_time_ms = 20;
	echo->f_delay_time_ms = *echo->delay_time_ms < 20 ? ECHO_DEFAULT_DELAY_TIME : (float )*echo->delay_time_ms;

	if ( *echo->feedback == 0 )
		*echo->feedback = (uint16_t )ECHO_DEFAULT_FEEDBACK * 100;
	echo->f_feedback = ((float )*echo->feedback*100.0F) > ECHO_MAX_FEEDBACK ? ECHO_MAX_FEEDBACK/100.0F : (float )*echo->feedback;

	if ( *echo->lfo_rate == 0 )
		*echo->lfo_rate = (uint16_t )ECHO_DEFAULT_LFO_RATE * 10;
	echo->f_lfo_rate = (float )*echo->lfo_rate / 10.0F;

	if ( *echo->lfo_depth_ms == 0 )
		*echo->lfo_depth_ms = (uint16_t )ECHO_DEFAULT_LFO_DEPTH;
	echo->f_lfo_depth_ms = (float )*echo->lfo_depth_ms;

	if ( *echo->cutoff == 0 )
		*echo->cutoff = (uint16_t )ECHO_DEFAULT_CUTOFF;
	echo->f_cutoff = (float )*echo->cutoff;

    // Precompute Moog g for feedback LPF
    float g = tanf(M_PI * echo->f_cutoff / echo->sample_rate);
    if (g > 10.0f)
    	g = 10.0f;
    echo->g = g;
    echo->k = 4.0f * 0.7f; // fixed resonance for warmth
}

ITCM_AREA_CODE void Effect_Space_Echo_Init(uint32_t *effect_s)
{
SPACE_ECHO_Effect_TypeDef *echo = (SPACE_ECHO_Effect_TypeDef *)effect_s;
	if (( echo->delay_time_ms == NULL ) || ( echo->feedback == NULL ) || ( echo->lfo_rate == NULL ) || ( echo->lfo_depth_ms == NULL ) || ( echo->cutoff == NULL ))
		return;
	if ( echo->block_size == 0 )
		echo->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	if ( echo->sample_rate == 0 )
		echo->sample_rate = Sound_Sample_Frequency;
    for (int i = 0; i < ECHO_BUFFER_SIZE; i++)
    	echo->buffer[i] = 0.0f;
    echo->write_ptr = 0;
    echo->lfo_phase = 0.0f;

    Effect_Space_Echo_Set_params(echo);
    echo->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_Space_Echo(uint32_t *effect_s)
{
uint32_t	i;
SPACE_ECHO_Effect_TypeDef *echo = (SPACE_ECHO_Effect_TypeDef *)effect_s;

	if ((( echo->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( echo == NULL ))
		return;
    Effect_Space_Echo_Set_params(echo);
	for ( i=0;i<echo->block_size;i++)
	{
		if (( echo->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			echo->out_buf[i] = space_echo_process(echo,__Q15_2_FLOAT(echo->in_buf[i]));
		else
			echo->out_buf[i]  = echo->in_buf[i];
	}
}

#endif // #ifdef SOUND_ENGINE_ENABLED
