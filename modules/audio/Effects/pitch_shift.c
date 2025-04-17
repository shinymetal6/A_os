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
 * pitch_shift.c
 *
 *  Created on: Apr 16, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"
#include "pitch_shift.h"

#include <math.h>

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define PI 3.14159265358979323846f
#define BUFFER_SIZE 128       // Input/output buffer size
#define GRAIN_SIZE 32         // Size of each grain
#define HOP_SIZE_IN 16        // Hop size for input grains
#define HOP_SIZE_OUT 24       // Hop size for output grains (controls pitch shift)

// Linear interpolation function
float interpolate(float *buffer, float position, int length) {
    int index = (int)position;
    float frac = position - (float)index;

    if (index < 0 || index >= length - 1) {
        return 0.0f; // Handle edge cases
    }

    return (1.0f - frac) * buffer[index] + frac * buffer[index + 1];
}

// Hann window coefficients for grain_size
void apply_hann_window(float *grain, int grain_size) {
    for (int i = 0; i < grain_size; i++) {
        float window_value = 0.5f * (1.0f - cosf(2.0f * PI * i / (grain_size - 1)));
        grain[i] *= window_value;
    }
}

// Granular synthesis pitch shifter
void granular_pitch_shift(float *input, float *output, int input_length, float pitch_factor) {
    int hop_size_out = (int)(HOP_SIZE_IN * pitch_factor); // Adjust output hop size based on pitch factor
    int num_grains = (input_length - GRAIN_SIZE) / HOP_SIZE_IN + 1;
    int output_index = 0;

    // Temporary buffer for grain processing
    float grain[GRAIN_SIZE];

    // Process grains
    for (int grain_idx = 0; grain_idx < num_grains; grain_idx++) {
        int input_pos = grain_idx * HOP_SIZE_IN;

        // Extract grain using linear interpolation
        for (int i = 0; i < GRAIN_SIZE; i++) {
            float read_pos = (float)(input_pos + i);
            grain[i] = interpolate(input, read_pos, input_length);
        }

        // Apply Hann window to the grain
        apply_hann_window(grain, GRAIN_SIZE);

        // Write grain to output with adjusted hop size
        for (int i = 0; i < GRAIN_SIZE; i++) {
            if (output_index + i < input_length) {
                output[output_index + i] += grain[i]; // Overlap-add
            }
        }

        // Advance output index
        output_index += hop_size_out;
    }
}

float input[BUFFER_SIZE];
float output[BUFFER_SIZE * 2] = {0}; // Output may be longer due to pitch shift
float pitch_factor = 1.5f; // Shift up by 1.5x

ITCM_AREA_CODE static void pitch_shift_init(PITCH_SHIFT_Effect_TypeDef *pitch_shift)
{

}
int16_t outputData_p[HALF_NUMBER_OF_AUDIO_SAMPLES];

ITCM_AREA_CODE void Do_PitchShift(int16_t *inputData, int16_t *outputData, uint8_t index)
{
PITCH_SHIFT_Effect_TypeDef			*pitch_shift	= (PITCH_SHIFT_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;
	if (( pitch_shift->flags & EFFECT_INITIALIZED) == 0)
	{
		pitch_shift_init(pitch_shift);
		pitch_shift->flags |= EFFECT_INITIALIZED;
	}
	if (( pitch_shift->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			input[i] = (float )inputData[i];
		}
		granular_pitch_shift(input, output, BUFFER_SIZE, pitch_factor);
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			outputData_p[i] = outputData[i] = (int16_t )output[i];
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			outputData[i] = inputData[i];
		}
	}
	/*
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( phaser->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
			outputData[i] = (int16_t ) phaser_effect(phaser,(float )inputData[i]);
		else
			outputData[i] = inputData[i];
	}
	*/
}
/*
int main() {
    // Example input buffer (128 samples of a sine wave)
    float input[BUFFER_SIZE];
    float output[BUFFER_SIZE * 2] = {0}; // Output may be longer due to pitch shift

    // Generate a sine wave as input
    float frequency = 440.0f; // A4 note
    float sample_rate = 44100.0f;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        input[i] = sinf(2.0f * PI * frequency * i / sample_rate);
    }

    // Pitch factor: >1 increases pitch, <1 decreases pitch
    float pitch_factor = 1.5f; // Shift up by 1.5x

    // Apply granular synthesis pitch shifting
    granular_pitch_shift(input, output, BUFFER_SIZE, pitch_factor);

    // Print the first few output samples
    printf("Pitch-shifted output:\n");
    for (int i = 0; i < 10; i++) {
        printf("output[%d] = %f\n", i, output[i]);
    }

    return 0;
}
*/
