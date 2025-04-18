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
 * chorus.c
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
#include "chorus.h"
#include "arm_math.h"


// Read from delay line with fractional delay (linear interpolation)
ITCM_AREA_CODE static float delay_line_read(CHORUS_Effect_TypeDef *chorus, float delay_samples)
{
	uint32_t index1 = (chorus->write_index - (int)delay_samples + CHORUS_BUFFER_SIZE) % CHORUS_BUFFER_SIZE;
	uint32_t index2 = (index1 - 1 + CHORUS_BUFFER_SIZE) % CHORUS_BUFFER_SIZE;
    float frac = delay_samples - (int)delay_samples;

    return (1.0f - frac) * chorus->buffer[index1] + frac * chorus->buffer[index2];
}

// Write to delay line
ITCM_AREA_CODE static void delay_line_write(CHORUS_Effect_TypeDef *chorus, float sample)
{
	chorus->buffer[chorus->write_index] = sample;
	chorus->write_index = (chorus->write_index + 1) % CHORUS_BUFFER_SIZE;
}

ITCM_AREA_CODE static void delay_line_init(CHORUS_Effect_TypeDef *chorus, int max_delay_samples)
{
uint32_t	i;
	chorus->write_index = 0;
	chorus->max_delay_samples = max_delay_samples;
    for (i = 0; i < CHORUS_BUFFER_SIZE; i++)
    	chorus->buffer[i] = 0.0f;
}

ITCM_AREA_CODE static float chorus_effect(CHORUS_Effect_TypeDef *chorus,float input)
{
    // Compute LFO-modulated delay time
	chorus->lfo_value = 0.5f * (1.0f + sinf(chorus->lfo_phase)); // LFO value between 0 and 1
    float delay_samples = chorus->lfo_value * chorus->max_delay_samples;

    // Read delayed sample
    float delayed_sample = delay_line_read(chorus, delay_samples);

    // Mix dry and wet signals
    float wet = input + chorus->feedback * delayed_sample;
    float output = chorus->mix * wet + (1.0f - chorus->mix) * input;

    // Write to delay line
    delay_line_write(chorus, wet);

    // Update LFO phase
    chorus->lfo_phase += 2.0f * PI * chorus->lfo_frequency / CHORUS_SAMPLE_RATE;
    if (chorus->lfo_phase >= 2.0f * PI)
    	chorus->lfo_phase -= 2.0f * PI;
    return output;
}

ITCM_AREA_CODE static void chorus_init(CHORUS_Effect_TypeDef *chorus)
{
	chorus->lfo_frequency = 0.5F;
	chorus->feedback = 0.3F;
	chorus->mix = 0.5F;
    delay_line_init(chorus, (int)(CHORUS_MAX_DELAY_MS * CHORUS_SAMPLE_RATE / 1000.0f));
}

ITCM_AREA_CODE void Do_Chorus(int16_t *inputData, int16_t *outputData, uint8_t index)
{
CHORUS_Effect_TypeDef			*chorus	= (CHORUS_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;
	if (( chorus->flags & EFFECT_INITIALIZED) == 0)
	{
		chorus_init(chorus);
		chorus->flags |= EFFECT_INITIALIZED;
	}
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( chorus->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
			outputData[i] = (int16_t ) chorus_effect(chorus,(float )inputData[i]);
		else
			outputData[i] = inputData[i];
	}
}

#endif
