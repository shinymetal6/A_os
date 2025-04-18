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
 * reverb.c
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
#include "arm_math.h"
#include "reverb.h"

ITCM_AREA_CODE static void comb_init(REVERB_CombFilter_TypeDef *filter, int delay_samples, float feedback)
{
    filter->write_index = 0;
    filter->delay_samples = delay_samples;
    filter->feedback = feedback;
    for (int i = 0; i < REVERB_BUFFER_SIZE; i++)
        filter->buffer[i] = 0.0f;
}

ITCM_AREA_CODE static  void allpass_init(REVERB_AllPassFilter_TypeDef *filter, int delay_samples)
{
    filter->delay_samples = delay_samples;
    filter->buffer = 0.0f;
}

ITCM_AREA_CODE static  float comb_process(REVERB_CombFilter_TypeDef *filter, float input_sample)
{
	filter->read_index = (filter->write_index - filter->delay_samples + REVERB_BUFFER_SIZE) % REVERB_BUFFER_SIZE;
    float output_sample = filter->buffer[filter->read_index] + input_sample;
    filter->buffer[filter->write_index] = input_sample + filter->feedback * output_sample;
    filter->write_index = (filter->write_index + 1) % REVERB_BUFFER_SIZE;
    return output_sample;
}

// Process a single sample through an all-pass filter
ITCM_AREA_CODE static  float reverb_allpass_process(REVERB_AllPassFilter_TypeDef *filter, float input_sample)
{
    float delayed_sample = filter->buffer;
    filter->buffer = input_sample;
    return delayed_sample - input_sample;
}

ITCM_AREA_CODE static  float reverb_effect(REVERB_Effect_TypeDef *reverb,float input)
{
uint32_t i;
REVERB_CombFilter_TypeDef		*comb_filters = reverb->REVERB_CombFilter;
REVERB_AllPassFilter_TypeDef	*allpass_filters = reverb->REVERB_AllPassFilter;

float wet = 0.0f;

	// Pass input through comb filters
	for (i = 0; i < REVERB_NUM_COMB; i++)
		wet += comb_process(&comb_filters[i], input);
	wet *= 0.25f; // Normalize output

	// Pass wet signal through all-pass filters
	for (i = 0; i < REVERB_NUM_ALLPASS; i++)
		wet = reverb_allpass_process(&allpass_filters[i], wet);

	// Mix dry and wet signals
	return  reverb->mix * wet + (1.0f - reverb->mix) * input;
}

ITCM_AREA_CODE static  void update_comb_filters(REVERB_CombFilter_TypeDef *comb_filters, float room_size)
{
uint32_t i;
    float base_delays[4] = {0.0297f, 0.0371f, 0.0411f, 0.0437f}; // Base delay times in seconds
    float feedback_gains[4] = {0.84f, 0.84f, 0.84f, 0.84f};       // Feedback gains

    for (i = 0; i < 4; i++)
    {
        comb_filters[i].delay_samples = (int)(REVERB_SAMPLE_RATE * base_delays[i] * room_size);
        comb_filters[i].feedback = feedback_gains[i];
    }
}

ITCM_AREA_CODE static  void reverb_init(REVERB_Effect_TypeDef *reverb)
{
uint32_t i;
REVERB_CombFilter_TypeDef		*comb_filters = reverb->REVERB_CombFilter;
REVERB_AllPassFilter_TypeDef	*allpass_filters = reverb->REVERB_AllPassFilter;
    for (i = 0; i < REVERB_NUM_COMB; i++)
        comb_init(&comb_filters[i], 0, 0.0f);
    allpass_init(&allpass_filters[0], (int)(REVERB_SAMPLE_RATE * 0.0050f));
    allpass_init(&allpass_filters[1], (int)(REVERB_SAMPLE_RATE * 0.0123f));
    reverb->room_size = REVERB_LARGE_ROOM;
}

ITCM_AREA_CODE void Do_Reverb(int16_t *inputData, int16_t *outputData, uint8_t index)
{
REVERB_Effect_TypeDef		*reverb	= (REVERB_Effect_TypeDef *)Effects[index].private_data;
REVERB_CombFilter_TypeDef	*comb_filters = reverb->REVERB_CombFilter;
uint32_t	i;

	if (( reverb->flags & EFFECT_INITIALIZED) == 0)
	{
		reverb_init(reverb);
		reverb->flags |= EFFECT_INITIALIZED;
	}

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( reverb->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
		{
		    // Update comb filters for the current room size
		    update_comb_filters(comb_filters, reverb->room_size);
			outputData[i] = (int16_t ) reverb_effect(reverb,(float )inputData[i]);
		}
		else
			outputData[i] = inputData[i];
	}
}

#endif
