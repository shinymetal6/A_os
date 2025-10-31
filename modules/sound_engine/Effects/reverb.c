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
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "reverb.h"

#define	REVERB_DELAY_AREA_CODE	__attribute__((section(".d2ram"))) __attribute__ ((aligned (32)))

REVERB_DELAY_AREA_CODE	Reverb_DelayLine_TypeDef reverb_comb_filters[REVERB_NUM_COMB_FILTERS];
/*REVERB_DELAY_AREA_CODE*/	Reverb_DelayLine_TypeDef reverb_allpass_filters[REVERB_NUM_ALLPASS_FILTERS]; // All-pass filters

// Initialize a delay line
ITCM_AREA_CODE static void reverb_delay_line_init(Reverb_DelayLine_TypeDef *delay_line, uint32_t delay_length)
{
    //memset(delay_line->buffer, 0, sizeof(delay_line->buffer));
    memset(delay_line, 0, sizeof(Reverb_DelayLine_TypeDef));
    delay_line->write_index = 0;
    delay_line->delay_length = delay_length;
}

// Process a sample through a delay line
ITCM_AREA_CODE static q15_t reverb_delay_line_process(Reverb_DelayLine_TypeDef *delay_line, q15_t input)
{
    // Read from the delay line
	uint32_t reverb_max_delay_len;
	Sound_Sample_Frequency <= DEFAULT_SAMPLE_FREQUENCY ? reverb_max_delay_len = (uint32_t )(Sound_Sample_Frequency/4.0F) : DEFAULT_SAMPLE_FREQUENCY/4;

    uint32_t read_index = (delay_line->write_index + reverb_max_delay_len - delay_line->delay_length) % reverb_max_delay_len;
    q15_t output = delay_line->buffer[read_index];

    // Write to the delay line
    delay_line->buffer[delay_line->write_index] = input;
    delay_line->write_index = (delay_line->write_index + 1) % reverb_max_delay_len;

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
        q15_t mixed_output = (q15_t )(float )(dry_sample * reverb->f_dry_mix) + (float )(wet_sample * reverb->f_wet_mix);

        // Store the output sample
        return mixed_output;
}

// Initialize the reverb
ITCM_AREA_CODE void Effect_Reverb_Init(uint32_t *effect_s)
{
REVERB_Effect_TypeDef *reverb = (REVERB_Effect_TypeDef *)effect_s;

    // Initialize comb filters with different delay lengths
	if ( reverb->block_size == 0 )
		reverb->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
    uint32_t comb_delay_lengths[REVERB_NUM_COMB_FILTERS] = {1557, 1617, 1491, 1422}; // Prime numbers for diffusion
    for (int i = 0; i < REVERB_NUM_COMB_FILTERS; i++) {
    	reverb->comb_filters[i] = reverb_comb_filters[i];
    	reverb_delay_line_init(&reverb->comb_filters[i], comb_delay_lengths[i]);
    }

    // Initialize all-pass filters with different delay lengths
    uint32_t allpass_delay_lengths[REVERB_NUM_ALLPASS_FILTERS] = {225, 556};
    for (int i = 0; i < REVERB_NUM_ALLPASS_FILTERS; i++) {
    	reverb->allpass_filters[i] = reverb_allpass_filters[i];
    	reverb_delay_line_init(&reverb->allpass_filters[i], allpass_delay_lengths[i]);
    }

    // Set feedback gain and mix levels
    reverb->feedback_gain_q15 = (q15_t)(reverb->f_feedback_gain * 32768.0f); // Convert to Q15
}

ITCM_AREA_CODE void Effect_Reverb(uint32_t *effect_s)
{
uint32_t	i;
REVERB_Effect_TypeDef *reverb = (REVERB_Effect_TypeDef *)effect_s;

	reverb->time_start = DWT->CYCCNT;
	for ( i=0;i<reverb->block_size;i++)
	{
		if (( reverb->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			reverb->out_buf[i]  = reverb_process(reverb,reverb->in_buf[i]);
		else
			reverb->out_buf[i]  = reverb->in_buf[i];

	}
	reverb->effect_time = (DWT->CYCCNT - reverb->time_start) / (HSI_CLOCK / 1000000);
}

#endif // #ifdef SOUND_ENGINE_ENABLED

