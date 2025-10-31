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
 * robot_voice.c
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
#include "robot_voice.h"

// Simple 2-pole bandpass filter (bilinear transform)
ITCM_AREA_CODE static void bp_filter_update_coeffs(ROBOT_VOICE_Effect_TypeDef *rv, float *a1, float *a2, float *b0, float *b1)
{
    float w0 = 2.0f * M_PI * rv->f_bp_freq / rv->sample_rate;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float alpha = sin_w0 / (2.0f * rv->f_bp_q);

    float a0 = 1.0f + alpha;
    *b0 = (0.5f * sin_w0) / a0;
    *b1 = 0.0f;
    *a1 = (-2.0f * cos_w0) / a0;
    *a2 = (1.0f - alpha) / a0;
}

ITCM_AREA_CODE static q15_t robot_process(ROBOT_VOICE_Effect_TypeDef *rv, float input)
{
    const float fs = rv->sample_rate;

    // --- 1. Ring Modulation ---
    float carrier;
    rv->f_ring_phase += rv->f_ring_freq / fs;
    if (rv->f_ring_phase >= 1.0f) rv->f_ring_phase -= 1.0f;

    if (rv->i_ring_wave == 0) {
        carrier = sinf(2.0f * M_PI * rv->f_ring_phase);
    } else {
        carrier = (rv->f_ring_phase < 0.5f) ? 1.0f : -1.0f; // square
    }

    float modulated = input * carrier;

    // --- 2. Bit Crushing ---
    float scale = powf(2.0f, rv->f_bit_depth - 1.0f); // e.g., 8-bit → 128
    float crushed = roundf(modulated * scale) / scale;

    // --- 3. Bandpass Filter (emulates vocal tract) ---
    float a1, a2, b0, b1;
    bp_filter_update_coeffs(rv, &a1, &a2, &b0, &b1);

    float bp_out = b0 * crushed + b1 * rv->f_bp_x1 - a1 * rv->f_bp_y1 - a2 * rv->f_bp_y2;

    // Update filter state
    rv->f_bp_x2 = rv->f_bp_x1;
    rv->f_bp_x1 = crushed;
    rv->f_bp_y2 = rv->f_bp_y1;
    rv->f_bp_y1 = bp_out;

    return __FLOAT_2_Q15(bp_out);
}

ITCM_AREA_CODE static void Effect_Robot_Set_Params(ROBOT_VOICE_Effect_TypeDef *rv)
{
    rv->f_ring_freq = (*rv->ring_freq < (uint16_t )ROBOT_VOICE_RING_FREQ_MIN) ? ROBOT_VOICE_RING_FREQ_MIN : (*rv->ring_freq > (uint16_t )ROBOT_VOICE_RING_FREQ_MAX ? ROBOT_VOICE_RING_FREQ_MAX : (float )*rv->ring_freq);
    rv->f_bit_depth = (*rv->bit_depth < 4) ? 4.0f : (*rv->bit_depth > 16 ? 16.0f : (float )*rv->bit_depth);
    rv->f_bp_freq = (*rv->bp_freq < 200) ? 200.0f : (*rv->bp_freq > 4000 ? 4000.0f : (float )*rv->bp_freq);
    rv->f_bp_q = (*rv->bp_q < 3) ? 0.3f : (*rv->bp_q > 30 ? 3.0f : (float )*rv->bp_q * 10.0F);

    rv->f_bp_x1 = rv->f_bp_x2 = 0.0f;
    rv->f_bp_y1 = rv->f_bp_y2 = 0.0f;
}

ITCM_AREA_CODE void Effect_Robot_Init(uint32_t *effect_s)
{
ROBOT_VOICE_Effect_TypeDef *rv = (ROBOT_VOICE_Effect_TypeDef *)effect_s;

	if (( rv->ring_freq == NULL ) || ( rv->bit_depth == NULL ) || ( rv->bp_freq == NULL ) || ( rv->bp_q == NULL ))
		return;
	if ( rv->block_size == 0 )
		rv->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	if ( rv->sample_rate == 0 )
		rv->sample_rate = Sound_Sample_Frequency;

    rv->f_ring_phase = 0.0f;

    //robot_set_params(rv, 600.0f, 1, 8.0f, 1200.0f, 1.2f); // default: square wave, 8-bit, 1.2kHz BP
    Effect_Robot_Set_Params(rv);
    rv->status |= SOUND_EFFECT_INITIALIZED;
}


ITCM_AREA_CODE void Effect_Robot(uint32_t *effect_s)
{
uint32_t	i;
ROBOT_VOICE_Effect_TypeDef *rv = (ROBOT_VOICE_Effect_TypeDef *)effect_s;

	if ((( rv->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( rv == NULL ))
		return;
	if (( rv->flags & RV_UPDATE_PARAMS) == RV_UPDATE_PARAMS)
			Effect_Robot_Set_Params(rv);
	rv->time_start = DWT->CYCCNT;
	for ( i=0;i<rv->block_size;i++)
	{
		if (( rv->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
		{
			rv->out_buf[i] = robot_process(rv,__Q15_2_FLOAT(rv->in_buf[i]));
		}
		else
			rv->out_buf[i]  = rv->in_buf[i];
	}
	rv->effect_time = (DWT->CYCCNT - rv->time_start) / (HSI_CLOCK / 1000000);
}

#endif // #ifdef SOUND_ENGINE_ENABLED

