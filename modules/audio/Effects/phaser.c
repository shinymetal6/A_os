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
 * phaser.c
 *
 *  Created on: Apr 7, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"
#include "phaser.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>




// Initialize an all-pass filter
void init_all_pass_filter(AllPassFilter_TypeDef *filter, int delay_samples) {
    filter->write_index = 0;
    filter->delay_samples = delay_samples;
    for (int i = 0; i < MAX_DELAY; i++) {
        filter->buffer[i] = 0.0f;
    }
}

// Process audio through an all-pass filter
float process_all_pass_filter(AllPassFilter_TypeDef *filter, float input) {
    int read_index = filter->write_index - filter->delay_samples;
    if (read_index < 0) {
        read_index += MAX_DELAY;
    }

    float buffered_value = filter->buffer[read_index];
    float output = buffered_value - input;
    filter->buffer[filter->write_index] = input + buffered_value;
    filter->write_index = (filter->write_index + 1) % MAX_DELAY;

    return output;
}



// Initialize the phaser effect
void init_phaser(Phaser_TypeDef *phaser, int sample_rate) {
    int base_delay = sample_rate / 1000; // Base delay time (e.g., 1ms)
    for (int i = 0; i < NUM_STAGES; i++) {
        init_all_pass_filter(&phaser->filters[i], base_delay + i * (base_delay / 2));
    }
    phaser->lfo_phase = 0.0f;
    phaser->feedback_buffer = 0.0f;
}

// Process audio through the phaser effect
float process_phaser(Phaser_TypeDef *phaser, float input, float lfo_freq, float depth, float feedback) {
    // Update LFO phase
    phaser->lfo_phase += 2.0f * M_PI * lfo_freq / PHASER_SAMPLE_RATE;
    if (phaser->lfo_phase > 2.0f * M_PI) {
        phaser->lfo_phase -= 2.0f * M_PI;
    }

    // Calculate modulated delay offset
    //float modulation = depth * sin(phaser->lfo_phase);

    // Process the input through the all-pass filters
    float wet = phaser->feedback_buffer + input;
    for (int i = 0; i < NUM_STAGES; i++) {
        wet = process_all_pass_filter(&phaser->filters[i], wet);
    }

    // Apply feedback
    phaser->feedback_buffer = wet * feedback;

    // Mix dry and wet signals
    float output = input + wet;

    return output;
}

Phaser_TypeDef phaser;

ITCM_AREA_CODE void Do_Phaser(int16_t *inputData, int16_t *outputData, uint8_t index)
{
PHASER_Effect_TypeDef			*PHASER_Effect 	= (PHASER_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;

	if (( PHASER_Effect->flags & PHASER_EFFECT_INITIALIZED) == 0)
	{
	    init_phaser(&phaser, PHASER_SAMPLE_RATE);
	    PHASER_Effect->flags |= PHASER_EFFECT_INITIALIZED;
	}
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( PHASER_Effect->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
			outputData[i] = (int16_t )(process_phaser(&phaser,(float )inputData[i], LFO_FREQ, DEPTH, FEEDBACK)) >> 4;
		else
			outputData[i] = inputData[i];
	}
}
// Example usage
/*
int main() {
    const int num_samples = 48000; // Process 1 second of audio

    Phaser phaser;
    init_phaser(&phaser, PHASER_SAMPLE_RATE);

    // Simulate processing audio samples
    for (int i = 0; i < num_samples; i++) {
        float input_sample = sin(2.0f * M_PI * 440.0f * i / PHASER_SAMPLE_RATE); // 440 Hz sine wave
        float output_sample = process_phaser(&phaser, input_sample, LFO_FREQ, DEPTH, FEEDBACK);

        // Output the processed sample (for example, write to a file or play back)
    }

    return 0;
}
*/
