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
 * small_buf_phaser.c
 *
 *  Created on: Apr 14, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"
#include "small_buf_phaser.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

AllPassFilter_typedef filters[SMALL_BUF_PHASER_NUM_STAGES];

// Initialize an all-pass filter
void init_small_buf_all_pass_filter(AllPassFilter_typedef *filter)
{
    filter->write_pos = 0;
    for (int i = 0; i < SMALL_BUF_PHASER_MAX_DELAY; i++) {
        filter->buffer[i] = 0.0f;
    }
}

// Process one sample through an all-pass filter
float process_small_buf_all_pass_filter(AllPassFilter_typedef *filter, float input, int delay_samples)
{
    int read_pos = filter->write_pos - delay_samples;
    if (read_pos < 0) read_pos += SMALL_BUF_PHASER_MAX_DELAY;

    float delayed_sample = filter->buffer[read_pos];
    float output = delayed_sample - input;
    filter->buffer[filter->write_pos] = input + 0.7f * delayed_sample; // Fixed feedback

    filter->write_pos = (filter->write_pos + 1) % SMALL_BUF_PHASER_MAX_DELAY;
    return output;
}

// Apply phaser effect to an audio buffer
void SmallBufPhaser(float *input, float *output, SmallBuf_PHASER_Effect_TypeDef *SmallBuf_PHASER_Effect)
{
    for (int i = 0; i < SMALL_BUF_PHASER_NUM_STAGES; i++)
    {
    	init_small_buf_all_pass_filter(&filters[i]);
    }

    float phase = 0.0f; // LFO phase accumulator
    for (int i = 0; i < SMALL_BUF_PHASER_SAMPLE_RATE; i++)
    {
        // Update LFO phase
        phase += SmallBuf_PHASER_Effect->lfo_rate;
        if (phase > 2 * SMALL_BUF_PHASER_PI) phase -= 2 * SMALL_BUF_PHASER_PI;

        // Calculate delay time based on LFO
        float delay_time = SmallBuf_PHASER_Effect->depth * (1 + sin(phase)) / 2.0f;
        int delay_samples = (int)(delay_time * SMALL_BUF_PHASER_SAMPLE_RATE);
        if (delay_samples >= SMALL_BUF_PHASER_MAX_DELAY)
        	delay_samples = SMALL_BUF_PHASER_MAX_DELAY - 1;

        // Process through all-pass filter stages
        float sample = input[i];
        for (int j = 0; j < SMALL_BUF_PHASER_NUM_STAGES; j++)
        {
            sample = process_small_buf_all_pass_filter(&filters[j], sample, delay_samples);
        }

        // Mix dry and wet signals
        output[i] = input[i] + sample; // Simple mix (adjustable)
    }
}

void SmallBufPhaser_passthrough(float *input, float *output, SmallBuf_PHASER_Effect_TypeDef *SmallBuf_PHASER_Effect)
{

}

ITCM_AREA_CODE void Do_SmallBufPhaser(int16_t *inputData, int16_t *outputData, uint8_t index)
{
SmallBuf_PHASER_Effect_TypeDef			*SmallBuf_PHASER_Effect 	= (SmallBuf_PHASER_Effect_TypeDef *)Effects[index].private_data;

	if (( SmallBuf_PHASER_Effect->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
		SmallBufPhaser((float *)inputData,(float *)outputData,SmallBuf_PHASER_Effect);
	else
		SmallBufPhaser_passthrough((float *)inputData,(float *)outputData,SmallBuf_PHASER_Effect);


}
