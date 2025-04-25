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
 *  Created on: Apr 25, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"

#include "reverb.h"

#define	REVERB_DELAY_AREA_CODE	__attribute__((section(".d2ram"))) __attribute__ ((aligned (32)))
REVERB_DELAY_AREA_CODE	q15_t reverb_delay_buffer[REVERB_MAX_DELAY_LENGTH];

#define REVERB_DELAY_RAM_START			_d2ram_start
// Initialize a delay line
ITCM_AREA_CODE static void reverb_delay_line_init(Reverb_DelayLine_TypeDef *delay_line, uint32_t delay_length)
{
	delay_line->buffer = reverb_delay_buffer;
    memset(delay_line->buffer, 0, REVERB_MAX_DELAY_LENGTH);
    delay_line->write_index = 0;
    delay_line->delay_length = delay_length;
}

// Process a sample through a delay line
ITCM_AREA_CODE static q15_t reverb_delay_line_process(Reverb_DelayLine_TypeDef *delay_line, q15_t input)
{
    // Read from the delay line
    uint32_t read_index = (delay_line->write_index + REVERB_MAX_DELAY_LENGTH - delay_line->delay_length) % REVERB_MAX_DELAY_LENGTH;
    q15_t output = delay_line->buffer[read_index];

    // Write to the delay line
    delay_line->buffer[delay_line->write_index] = input;
    delay_line->write_index = (delay_line->write_index + 1) % REVERB_MAX_DELAY_LENGTH;

    return output;
}

ITCM_AREA_CODE static q15_t reverb_process(REVERB_Effect_TypeDef *reverb, q15_t dry_sample)
{
        q15_t wet_sample = 0;

        // Process through comb filters
        for (int c = 0; c < REVERB_NUM_COMB_FILTERS; c++) {
            q15_t comb_output = reverb_delay_line_process(&reverb->comb_filters[c], dry_sample);
            wet_sample += comb_output;

            // Apply feedback
            q15_t feedback = (q15_t)(((q31_t)comb_output * reverb->feedback_gain_q15) >> 15);
            reverb->comb_filters[c].buffer[reverb->comb_filters[c].write_index] = feedback;
        }

        // Process through all-pass filters
        for (int a = 0; a < REVERB_NUM_ALLPASS_FILTERS; a++) {
            wet_sample = reverb_delay_line_process(&reverb->allpass_filters[a], wet_sample);
        }

        // Mix dry and wet signals
        q15_t mixed_output = (q15_t)((((q31_t)dry_sample * reverb->mix_dry_q15) >> 15) +
                                     (((q31_t)wet_sample * reverb->mix_wet_q15) >> 15));

        // Clamp to prevent overflow
        if (mixed_output > 32767) mixed_output = 32767;
        if (mixed_output < -32768) mixed_output = -32768;

        // Store the output sample
        return mixed_output;
}

// Initialize the reverb
ITCM_AREA_CODE void Effect_Reverb_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
REVERB_Effect_TypeDef *reverb = (REVERB_Effect_TypeDef *)effect->private_data;

    // Initialize comb filters with different delay lengths
    uint32_t comb_delay_lengths[REVERB_NUM_COMB_FILTERS] = {1557, 1617, 1491, 1422}; // Prime numbers for diffusion
    for (int i = 0; i < REVERB_NUM_COMB_FILTERS; i++) {
    	reverb_delay_line_init(&reverb->comb_filters[i], comb_delay_lengths[i]);
    }

    // Initialize all-pass filters with different delay lengths
    uint32_t allpass_delay_lengths[REVERB_NUM_ALLPASS_FILTERS] = {225, 556};
    for (int i = 0; i < REVERB_NUM_ALLPASS_FILTERS; i++) {
    	reverb_delay_line_init(&reverb->allpass_filters[i], allpass_delay_lengths[i]);
    }

    // Set feedback gain and mix levels
    reverb->feedback_gain_q15 = (q15_t)(reverb->feedback_gain * 32768.0f); // Convert to Q15
    reverb->mix_dry_q15 = (q15_t)(reverb->dry_mix * 32768.0f);             // Dry mix in Q15
    reverb->mix_wet_q15 = (q15_t)(reverb->wet_mix * 32768.0f);             // Wet mix in Q15
}

ITCM_AREA_CODE void Effect_Reverb(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
REVERB_Effect_TypeDef *reverb = (REVERB_Effect_TypeDef *)effect->private_data;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( effect->status & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			effect->out_buf[i + start_sample]  = reverb_process(reverb,effect->in_buf[i])+effect->out_device;
		else
			effect->out_buf[i + start_sample]  = effect->in_buf[i]+effect->out_device;

	}
}
