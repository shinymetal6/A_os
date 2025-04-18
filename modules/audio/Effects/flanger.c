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
 * flanger.c
 *
 *  Created on: Apr 18, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
//#include "../../kernel/kernel_opt.h"

#include "../audio.h"
#ifdef AUDIO_GENERATORS_ENABLED
#include "../effects.h"
#include "flanger.h"
#include "arm_math.h"

ITCM_AREA_CODE static void flanger_delay_line_init(FLANGER_DelayLine_TypeDef *delay_line, int max_delay_samples)
{
    delay_line->write_index = 0;
    delay_line->max_delay_samples = max_delay_samples;
    for (int i = 0; i < FLANGER_BUFFER_SIZE; i++)
        delay_line->buffer[i] = 0.0f;
}

ITCM_AREA_CODE static void flanger_init(FLANGER_Effect_TypeDef *flanger)
{
	FLANGER_DelayLine_TypeDef 	*delay_line = (FLANGER_DelayLine_TypeDef *)flanger->delay_line;
	flanger->lfo_frequency = 0.5F;
	flanger->feedback = 0.7F;
	flanger->mix = 0.5F;
	flanger_delay_line_init(delay_line, (int)(FLANGER_MAX_DELAY_MS * FLANGER_SAMPLE_RATE / 1000.0f));
}

// Read from delay line with fractional delay (linear interpolation)
ITCM_AREA_CODE static float flanger_delay_line_read(FLANGER_DelayLine_TypeDef *delay_line, float delay_samples)
{
    int index1 = (delay_line->write_index - (int)delay_samples + FLANGER_BUFFER_SIZE) % FLANGER_BUFFER_SIZE;
    int index2 = (index1 - 1 + FLANGER_BUFFER_SIZE) % FLANGER_BUFFER_SIZE;
    float frac = delay_samples - (int)delay_samples;

    return (1.0f - frac) * delay_line->buffer[index1] + frac * delay_line->buffer[index2];
}

// Write to delay line
ITCM_AREA_CODE static void flanger_delay_line_write(FLANGER_DelayLine_TypeDef *delay_line, float sample)
{
    delay_line->buffer[delay_line->write_index] = sample;
    delay_line->write_index = (delay_line->write_index + 1) % FLANGER_BUFFER_SIZE;
}

// Process one sample
ITCM_AREA_CODE static float flanger_effect(FLANGER_Effect_TypeDef *flanger,float input)
{
FLANGER_DelayLine_TypeDef 	*delay_line = (FLANGER_DelayLine_TypeDef *)flanger->delay_line;
// Compute LFO-modulated delay time
	flanger->lfo_value = 0.5f * (1.0f + sinf(flanger->lfo_phase)); // LFO value between 0 and 1
	float delay_samples = flanger->lfo_value * delay_line->max_delay_samples;

	// Read delayed sample
	float delayed_sample = flanger_delay_line_read(delay_line, delay_samples);

	// Mix dry and wet signals
	float wet = input + flanger->feedback * delayed_sample;
	float output = flanger->mix * wet + (1.0f - flanger->mix) * input;

	// Write to delay line
	flanger_delay_line_write(delay_line, wet);

	// Update LFO phase
	flanger->lfo_phase += 2.0f * PI * flanger->lfo_frequency / FLANGER_SAMPLE_RATE;
	if (flanger->lfo_phase >= 2.0f * PI)
		flanger->lfo_phase -= 2.0f * PI;
    return output;
}

ITCM_AREA_CODE void Do_Flanger(int16_t *inputData, int16_t *outputData, uint8_t index)
{
FLANGER_Effect_TypeDef			*flanger	= (FLANGER_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;
	if (( flanger->flags & EFFECT_INITIALIZED) == 0)
	{
		flanger_init(flanger);
		flanger->flags |= EFFECT_INITIALIZED;
	}
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( flanger->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
			outputData[i] = (int16_t ) flanger_effect(flanger,(float )inputData[i]);
		else
			outputData[i] = inputData[i];
	}
}

#endif
