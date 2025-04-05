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

#include <stdlib.h>
#include <string.h>
#include <math.h>

// Different delay lengths (prime numbers to avoid comb filtering) default values
int 	lengths[REVERB_NUM_DELAY_LINES] = {1433, 1787, 2099, 2333};
float 	feedbacks[REVERB_NUM_DELAY_LINES] = {0.7f, 0.7f, 0.7f, 0.7f};
float 	gains[REVERB_NUM_DELAY_LINES] = {0.6f, 0.6f, 0.6f, 0.6f};
float	default_decayTime = 0.5F;
float	default_mix = 0.3F;

// Initialize a delay line
ITCM_AREA_CODE void initDelayLine(Reverb_DelayLine_typedef *line, int length, float feedback, float gain) {
    line->buffer = (float*)calloc(length, sizeof(float));
    line->length = length;
    line->position = 0;
    line->feedback = feedback;
    line->gain = gain;
}

// Initialize the reverb
ITCM_AREA_CODE void initReverb( uint8_t index)
{
REVERB_Effect_TypeDef			*REVERB_Effect 	= (REVERB_Effect_TypeDef *)Effects[index].private_data;
Reverb_effect_internals_typedef	*reverb 		= 	(Reverb_effect_internals_typedef *)REVERB_Effect->reverb;
uint32_t	i;
    // Scale delay times based on sample rate
    float scale = REVERB_Effect->sampleRate / 44100.0F;

    reverb->mix = REVERB_Effect->mix;
    reverb->decay = REVERB_Effect->decayTime;
    for(i=0;i<REVERB_NUM_DELAY_LINES;i++)
    {
        REVERB_Effect->lengths[i] = lengths[i];
        REVERB_Effect->feedbacks[i] = feedbacks[i];
        REVERB_Effect->gains[i] = gains[i];
    }

    for (i = 0; i < REVERB_NUM_DELAY_LINES; i++)
    {
        int scaledLength = (int)(REVERB_Effect->lengths[i] * scale);
        initDelayLine(&reverb->lines[i], scaledLength, REVERB_Effect->feedbacks[i], REVERB_Effect->gains[i]);
    }
}

// Process one sample through the reverb
ITCM_AREA_CODE float processReverb(Reverb_effect_internals_typedef *reverb, float input) {
float output = 0.0f;
float wet = 0.0f;
float dry = input;

    // Process each delay line
    for (int i = 0; i < REVERB_NUM_DELAY_LINES; i++)
    {
    	Reverb_DelayLine_typedef *line = &reverb->lines[i];

        // Read from delay line
        int readPos = (line->position - line->length + line->length) % line->length;
        float delayed = line->buffer[readPos] * line->gain;

        // Write to delay line (input + feedback from other lines)
        line->buffer[line->position] = input + delayed * line->feedback;

        // Update position
        line->position = (line->position + 1) % line->length;

        // Accumulate output
        wet += delayed;
    }

    // Normalize wet signal
    wet /= (float)REVERB_NUM_DELAY_LINES;

    // Mix dry and wet signals
    output = dry * (1.0f - reverb->mix) + wet * reverb->mix;

    return output;
}

// Free allocated memory
ITCM_AREA_CODE void freeReverb(uint8_t index)
{
REVERB_Effect_TypeDef			*REVERB_Effect 	= (REVERB_Effect_TypeDef *)Effects[index].private_data;
Reverb_effect_internals_typedef	*reverb 		= 	(Reverb_effect_internals_typedef *)REVERB_Effect->reverb;
	for (int i = 0; i < REVERB_NUM_DELAY_LINES; i++)
    {
        free(reverb->lines[i].buffer);
    }
}

ITCM_AREA_CODE void Do_Reverb(int16_t *inputData, int16_t *outputData, uint8_t index)
{
REVERB_Effect_TypeDef			*REVERB_Effect 	= (REVERB_Effect_TypeDef *)Effects[index].private_data;
Reverb_effect_internals_typedef	*reverb 		= 	(Reverb_effect_internals_typedef *)REVERB_Effect->reverb;
uint32_t	i;

	if (( REVERB_Effect->flags & REVERB_EFFECT_INITIALIZED) == 0)
	{
		REVERB_Effect->sampleRate = (float )DEFAULT_SAMPLE_FREQUENCY;
		REVERB_Effect->decayTime = default_decayTime;
		REVERB_Effect->mix = default_mix;
		initReverb(index);
		REVERB_Effect->flags |= REVERB_EFFECT_INITIALIZED;
	}
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( REVERB_Effect->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
			outputData[i] = (int16_t )processReverb(reverb,(float )inputData[i]);
		else
				outputData[i] = inputData[i];
	}
}

ITCM_AREA_CODE uint32_t Set_Params_Reverb(int32_t *param_struct, uint8_t index)
{
uint32_t	i;
Reverb_Params_typedef	*params = (Reverb_Params_typedef *)param_struct;
REVERB_Effect_TypeDef	*REVERB_Effect 	= (REVERB_Effect_TypeDef *)Effects[index].private_data;
Reverb_effect_internals_typedef	*reverb 		= 	(Reverb_effect_internals_typedef *)REVERB_Effect->reverb;

	freeReverb(index);
    for(i=0;i<REVERB_NUM_DELAY_LINES;i++)
    {
        REVERB_Effect->lengths[i] = params->length[i];
        REVERB_Effect->feedbacks[i] = params->feedback[i];
        REVERB_Effect->gains[i] = params->gain[i];
    }
    reverb->mix = params->mix;
    reverb->decay = params->decayTime;

    float scale = REVERB_Effect->sampleRate / 44100.0F;

    for (i = 0; i < REVERB_NUM_DELAY_LINES; i++)
    {
        int scaledLength = (int)(REVERB_Effect->lengths[i] * scale);
        initDelayLine(&reverb->lines[i], scaledLength, REVERB_Effect->feedbacks[i], REVERB_Effect->gains[i]);
    }
	return 0;
}


// Example usage:
/*
Reverb reverb;
initReverb(&reverb, 44100.0f, 0.5f, 0.3f);

// Process a buffer of samples
for (int i = 0; i < numSamples; i++) {
    output[i] = processReverb(&reverb, input[i]);
}

freeReverb(&reverb);
*/
/*
How It Works

    Delay Lines: The reverb uses multiple delay lines (4 in this case) with different lengths to create a dense reverberation effect.

    Feedback Network: Each delay line feeds back into itself with some attenuation to create the decay characteristic of reverb.

    Parameters:

        sampleRate: The sample rate of your audio system (e.g., 44100 Hz)

        decayTime: How long the reverb tail lasts (in seconds)

        mix: Blend between dry (original) and wet (reverb) signal (0.0 to 1.0)

    Processing: For each audio sample, the function:

        Reads from all delay lines

        Writes new values to the delay lines

        Mixes the dry and wet signals

Improvements You Could Make

    Add low-pass filtering in the feedback path to simulate high-frequency decay

    Implement early reflections before the diffuse reverb tail

    Add modulation to the delay lines to reduce metallic artifacts

    Implement a more sophisticated mixing matrix between delay lines

This is a basic implementation that should work well for many applications, but professional reverbs are typically much more complex.
*/
