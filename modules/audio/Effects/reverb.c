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
 *  Created on: Apr 5, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"
#include "reverb.h"

#include <math.h>

// Comb filter function
float reverb_comb_filter(REVERB_Effect_TypeDef *reverb,float input, int delay, float feedback_gain) {
	reverb->read_pos = (reverb->write_pos - delay + REVERB_BUFFER_SIZE) % REVERB_BUFFER_SIZE;
    float delayed = reverb->buffer[reverb->read_pos];
    float output = input + feedback_gain * delayed;
    reverb->buffer[reverb->write_pos] = output; // Store the output in the reverb->buffer
    return output;
}

// All-pass filter function
float reverb_all_pass_filter(REVERB_Effect_TypeDef *reverb,float input, int delay, float feedback_gain)
{
	reverb->read_pos = (reverb->write_pos - delay + REVERB_BUFFER_SIZE) % REVERB_BUFFER_SIZE;
    float delayed = reverb->buffer[reverb->read_pos];
    float output = feedback_gain * input + delayed - feedback_gain * reverb->buffer[reverb->write_pos];
    reverb->buffer[reverb->write_pos] = input;
    reverb->write_pos = (reverb->write_pos + 1) % REVERB_BUFFER_SIZE; // Circular reverb->buffer
    return output;
}

// Process one sample
float reverb_effect(REVERB_Effect_TypeDef *reverb,float input)
{
    float wet = 0.0f;

    // Apply multiple comb filters
    for (int i = 0; i < 6; i++)
    {
        wet += reverb_comb_filter(reverb,input, reverb->comb_delays[i], reverb->comb_gains[i]);
    }

    // Average the outputs of the comb filters
    wet /= 6.0f; // Divide by 6 (number of comb filters)

    // Apply all-pass filter to smooth echoes
    wet = reverb_all_pass_filter(reverb,wet, reverb->allpass_delay, reverb->allpass_feedback_gain);

    // Mix dry and wet signals
    return reverb->mix * input + (1.0F - reverb->mix) * wet;  // Simple equal mix
}

ITCM_AREA_CODE void reverb_init(REVERB_Effect_TypeDef *reverb)
{
	reverb->comb_gains[0] = 0.75f;
	reverb->comb_gains[1] = 0.7f;
	reverb->comb_gains[2] = 0.65f;
	reverb->comb_gains[3] = 0.6f;
	reverb->comb_gains[4] = 0.55f;
	reverb->comb_gains[5] = 0.5f;

	reverb->comb_delays[0] = 21;
	reverb->comb_delays[1] = 29;
	reverb->comb_delays[2] = 37;
	reverb->comb_delays[3] = 43;
	reverb->comb_delays[4] = 53;
	reverb->comb_delays[5] = 61;

	reverb->allpass_feedback_gain = REVERB_FIXED_ALLPASS_GAIN; // Feedback gain for the all-pass filter
	reverb->allpass_delay = 13;             // Delay length for the all-pass filter
}


ITCM_AREA_CODE void Do_Reverb(int16_t *inputData, int16_t *outputData, uint8_t index)
{
REVERB_Effect_TypeDef	*reverb	= (REVERB_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;

	if (( reverb->flags & EFFECT_INITIALIZED) == 0)
	{
		reverb_init(reverb);
		reverb->flags |= EFFECT_INITIALIZED;
	}
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( reverb->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
			outputData[i] = (int16_t ) reverb_effect(reverb,(float )inputData[i]);
		else
				outputData[i] = inputData[i];
	}
}
