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

#include <stdint.h>
#include <math.h>

// Low-frequency oscillator (LFO) function
ITCM_AREA_CODE static float sine_wave_lfo(float phase)
{
    return sinf(phase); // Generate a sine wave
}

// All-pass filter function
ITCM_AREA_CODE static float phaser_all_pass_filter(PHASER_Effect_TypeDef *phaser,float input, float feedback)
{
    float delayed = phaser->buffer[phaser->write_pos];
    float output = feedback * input + delayed - feedback * phaser->buffer[phaser->write_pos];
    phaser->buffer[phaser->write_pos] = input;
    phaser->write_pos = (phaser->write_pos + 1) % PHASER_BUFFER_SIZE; // Circular buffer
    return output;
}

// Process one sample
ITCM_AREA_CODE static float phaser_effect(PHASER_Effect_TypeDef *phaser,float input)
{
    // Update LFO phase
	phaser->lfo_phase += phaser->lfo_internal_rate; // Increment phase (adjust for desired LFO rate) , 0.001f default
    if (phaser->lfo_phase >= 2.0f * M_PI)
    	phaser->lfo_phase -= 2.0f * M_PI;

    // Generate LFO signal (sinusoidal modulation)
    float lfo_signal = sine_wave_lfo(phaser->lfo_phase);

    // Modulate feedback coefficient
    float feedback = 0.7f + 0.3f * lfo_signal; // Base + modulation

    // Apply cascade of all-pass filters
    float wet = input;
    for (int i = 0; i < 6; i++) { // Cascade of 6 all-pass filters
        wet = phaser_all_pass_filter(phaser,wet, feedback);
    }

    // Mix dry and wet signals
    return phaser->mix * input + (1.0F - phaser->mix) * wet;  // Simple equal mix

}

ITCM_AREA_CODE static void phaser_init(PHASER_Effect_TypeDef *phaser)
{
	phaser->lfo_phase = 0.0F;
	if ( phaser->lfo_rate == 0.0F )
		phaser->lfo_internal_rate = 0.001F;
	else
		phaser->lfo_internal_rate = phaser->lfo_rate / 1000.0F;

	phaser->mix = 0.5F;
}

ITCM_AREA_CODE void Do_Phaser(int16_t *inputData, int16_t *outputData, uint8_t index)
{
PHASER_Effect_TypeDef			*phaser	= (PHASER_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;
	if (( phaser->flags & EFFECT_INITIALIZED) == 0)
	{
		phaser_init(phaser);
		phaser->flags |= EFFECT_INITIALIZED;
	}
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( phaser->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
			outputData[i] = (int16_t ) phaser_effect(phaser,(float )inputData[i]);
		else
			outputData[i] = inputData[i];
	}
}
