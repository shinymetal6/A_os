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
#include "reverb.h"

// Initialize comb filter
void reverb_comb_init_q15(REVERB_CombFilter_TypeDef *filter, int delay_samples, q15_t feedback)
{
uint32_t	i;
    filter->write_index = 0;
    filter->delay_samples = delay_samples;
    filter->feedback = feedback;
    for (i = 0; i < REVERB_BUFFER_SIZE; i++)
        filter->buffer[i] = 0;
}

// Process a single sample through a comb filter (Q15)
q15_t reverb_comb_process_q15(REVERB_CombFilter_TypeDef *filter, q15_t input_sample)
{
	uint32_t read_index = (filter->write_index - filter->delay_samples + REVERB_BUFFER_SIZE) % REVERB_BUFFER_SIZE;
    q15_t output_sample = filter->buffer[read_index] + input_sample;
    filter->buffer[filter->write_index] = input_sample + ((q31_t)filter->feedback * output_sample >> 15);
    filter->write_index = (filter->write_index + 1) % REVERB_BUFFER_SIZE;
    return output_sample;
}

// Initialize all-pass filter
void reverb_allpass_init_q15(REVERB_AllPassFilter_TypeDef *filter, int delay_samples)
{
uint32_t	i;
    filter->write_index = 0;
    filter->delay_samples = delay_samples;
    for (i = 0; i < REVERB_BUFFER_SIZE; i++)
        filter->buffer[i] = 0;
}

// Process a single sample through an all-pass filter (Q15)
q15_t reverb_allpass_process_q15(REVERB_AllPassFilter_TypeDef *filter, q15_t input_sample)
{
    int read_index = (filter->write_index - filter->delay_samples + REVERB_BUFFER_SIZE) % REVERB_BUFFER_SIZE;
    q15_t delayed_sample = filter->buffer[read_index];
    filter->buffer[filter->write_index] = input_sample;
    filter->write_index = (filter->write_index + 1) % REVERB_BUFFER_SIZE;
    return delayed_sample - input_sample;
}

// Update comb filters for a given room size
void reverb_update_comb_filters_q15(REVERB_Effect_TypeDef *reverb)
{
REVERB_CombFilter_TypeDef		*comb_filters = reverb->REVERB_CombFilter;

    for (int i = 0; i < REVERB_NUM_COMB; i++) {
        comb_filters[i].delay_samples = (int)(REVERB_SAMPLE_RATE * reverb->base_delays[i] * reverb->room_size);
        comb_filters[i].feedback = (q15_t)(reverb->feedback_gains[i] * 32768.0f);
    }
}

// Reverb effect using Q15 fixed-point arithmetic
void reverb_process_q15(REVERB_Effect_TypeDef *reverb,q15_t *input, q15_t *output)
{
    // Update comb filters for the current room size
    reverb_update_comb_filters_q15(reverb);

    for (int i = 0; i < REVERB_BUFFER_SIZE; i++) {
        // Pass input sample through comb filters
        q15_t wet = 0;
        for (int j = 0; j < REVERB_NUM_COMB; j++) {
            wet += reverb_comb_process_q15(&reverb->REVERB_CombFilter[j], input[i]);
        }
        wet >>= 2; // Normalize output by dividing by the number of comb filters

        // Pass wet signal through all-pass filters
        for (int j = 0; j < REVERB_NUM_ALLPASS; j++) {
            wet = reverb_allpass_process_q15(&reverb->REVERB_AllPassFilter[j], wet);
        }

        // Mix dry and wet signals
        output[i] = ((q31_t)reverb->mix * wet + ((q31_t)(32768 - reverb->mix) * input[i])) >> 15;
    }
}


ITCM_AREA_CODE static  void reverb_init(REVERB_Effect_TypeDef *reverb)
{
uint32_t i;
REVERB_CombFilter_TypeDef		*comb_filters = reverb->REVERB_CombFilter;
REVERB_AllPassFilter_TypeDef	*allpass_filters = reverb->REVERB_AllPassFilter;
	for (i = 0; i < REVERB_NUM_COMB; i++)
		reverb_comb_init_q15(&comb_filters[i], 0, 0); // Placeholder initialization

	reverb_allpass_init_q15(&allpass_filters[0], (int)(REVERB_SAMPLE_RATE * 0.0050f));
	reverb_allpass_init_q15(&allpass_filters[1], (int)(REVERB_SAMPLE_RATE * 0.0123f));
    reverb->room_size = REVERB_LARGE_ROOM;
    reverb->base_delays[0] = 0.0297f;
    reverb->base_delays[1] = 0.0371f;
    reverb->base_delays[2] = 0.0411f;
    reverb->base_delays[3] = 0.0437f;
#define	FBK_GAIN	0.84f
    reverb->feedback_gains[0] = reverb->feedback_gains[1] = reverb->feedback_gains[2] = reverb->feedback_gains[3] = FBK_GAIN;


}

ITCM_AREA_CODE void Do_Reverb(int16_t *inputData, int16_t *outputData, uint8_t index)
{
REVERB_Effect_TypeDef		*reverb	= (REVERB_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;

	if (( reverb->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
	{
		if (( reverb->flags & EFFECT_INITIALIZED) == 0)
		{
			reverb_init(reverb);
			reverb->flags |= EFFECT_INITIALIZED;
		}
		reverb_process_q15(reverb,inputData,outputData);
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

#endif
