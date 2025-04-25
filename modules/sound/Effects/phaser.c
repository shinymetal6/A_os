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
 *  Created on: Apr 25, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"

#include "phaser.h"
// Process a block of audio samples through the phaser

ITCM_AREA_CODE static q15_t phaser_process(PHASER_Effect_TypeDef *phaser, q15_t sample)
{
q15_t feedback_sample = 0;

	// Apply feedback from the last stage
	if (PHASER_NUM_STAGES > 0)
		feedback_sample = (q15_t)(((q31_t)phaser->stages[PHASER_NUM_STAGES - 1].prev_out * phaser->feedback) >> 15);

	// Add feedback to the input sample
	q15_t input_with_feedback = (q15_t)((q31_t)sample + feedback_sample);

	// Process through all-pass filter stages
	for (int s = 0; s < PHASER_NUM_STAGES; s++)
	{
		Phaser_AllPassFilter_TypeDef *stage = &phaser->stages[s];

		// Calculate the feedback coefficient 'a' using LFO
		float lfo_value = 0.5f * (1.0f + sinf(2.0f * 3.14159265358979323846f * phaser->lfo_phase));
		q15_t a = (q15_t)(lfo_value * 32768.0f); // Convert to Q15

		// All-pass filter equation: y[n] = a * x[n] + x[n-1] - a * y[n-1]
		q31_t temp = ((q31_t)a * input_with_feedback) >> 15;
		q15_t output_sample = (q15_t)(temp + stage->prev_in - ((q31_t)a * stage->prev_out >> 15));

		// Update filter state
		stage->prev_in = input_with_feedback;
		stage->prev_out = output_sample;

		// Pass the output of this stage to the next stage
		input_with_feedback = output_sample;
	}

	// Mix dry and wet signals
	q15_t dry = sample;
	q15_t wet = input_with_feedback;
	q15_t mixed_output = (q15_t)(((q31_t)dry + (q31_t)wet) >> 1); // Equal mix (50% dry, 50% wet)

	// Clamp to prevent overflow
	if (mixed_output > 32767) mixed_output = 32767;
	if (mixed_output < -32768) mixed_output = -32768;

	// Store the output sample

	// Update LFO phase
	phaser->lfo_phase += phaser->lfo_increment;
	if (phaser->lfo_phase >= 1.0f)
		phaser->lfo_phase -= 1.0f;
	return mixed_output;
}

// Initialize the phaser
ITCM_AREA_CODE void Effect_Phaser_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
PHASER_Effect_TypeDef *phaser = (PHASER_Effect_TypeDef *)effect->private_data;


    // Initialize all-pass filter stages
    for (int i = 0; i < PHASER_NUM_STAGES; i++)
    {
        phaser->stages[i].prev_in = 0;
        phaser->stages[i].prev_out = 0;
    }

    // Initialize LFO
    phaser->lfo_phase = 0.0f;
    phaser->lfo_increment = phaser->lfo_frequency / PHASER_SAMPLE_RATE;

    // Set feedback gain
    phaser->feedback = (q15_t)(phaser->feedback_gain * 32768.0f); // Convert to Q15
}


ITCM_AREA_CODE void Effect_Phaser(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
PHASER_Effect_TypeDef *phaser = (PHASER_Effect_TypeDef *)effect->private_data;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( effect->status & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			effect->out_buf[i + start_sample]  = phaser_process(phaser,effect->in_buf[i])+effect->out_device;
		else
			effect->out_buf[i + start_sample]  = effect->in_buf[i]+effect->out_device;

	}
}
