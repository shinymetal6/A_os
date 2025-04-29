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
 * chorus.c
 *
 *  Created on: Apr 25, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"

#include "chorus.h"
/*
#define	CHORUS_DELAY_AREA_CODE	__attribute__((section(".d2ram"))) __attribute__ ((aligned (32)))
CHORUS_DELAY_AREA_CODE	q15_t chorus_delay_buffer[CHORUS_MAX_DELAY_LENGTH];

// Initialize the delay line
ITCM_AREA_CODE static void chorus_delay_line_init(Chorus_DelayLine_TypeDef *delay_line, uint32_t delay_length) {
	delay_line->buffer = chorus_delay_buffer;
    memset(delay_line->buffer, 0, CHORUS_MAX_DELAY_LENGTH);
    delay_line->write_index = 0;
    delay_line->delay_length = delay_length;
}

// Process a sample through the delay line
ITCM_AREA_CODE static q15_t chorus_delay_line_process(Chorus_DelayLine_TypeDef *delay_line, q15_t input) {
    // Read from the delay line
    uint32_t read_index = (delay_line->write_index + CHORUS_MAX_DELAY_LENGTH - delay_line->delay_length) % CHORUS_MAX_DELAY_LENGTH;
    q15_t output = delay_line->buffer[read_index];

    // Write to the delay line
    delay_line->buffer[delay_line->write_index] = input;
    delay_line->write_index = (delay_line->write_index + 1) % CHORUS_MAX_DELAY_LENGTH;

    return output;
}
*/
// Generate LFO output
ITCM_AREA_CODE static q15_t chorus_lfo_generate(q15_t phase_increment, q15_t *lfo_phase)
{
    q15_t output = arm_sin_q15(*lfo_phase); // Generate sine wave
    *lfo_phase += phase_increment;         // Update phase
    *lfo_phase &= 0x7FFF;                   // Wrap phase within [0, 2π]
    return output;
}

// Process a block of audio samples through the chorus effect
ITCM_AREA_CODE static void chorus_process(Chorus_Effect_TypeDef *chorus, q15_t *input, q15_t *output, uint32_t out_device, uint32_t block_size)
{
	// Compute LFO phase increment (frequency in Q15 format)
	q15_t lfo_phase_increment = (q15_t)(CHORUS_LFO_FREQUENCY * 32768.0f / CHORUS_SAMPLE_RATE);

	for (int i = 0; i < block_size; i++) {
		// Generate LFO output
		q15_t lfo_output = chorus_lfo_generate(lfo_phase_increment, &chorus->lfo_phase);

		// Modulate delay length with LFO
		int32_t base_delay = CHORUS_SAMPLE_RATE / 10; // Base delay length (100 ms)
		int32_t modulated_delay = base_delay + (int32_t)((float)lfo_output * base_delay / 32768.0f);
		if (modulated_delay < 0) modulated_delay = 0;
		if (modulated_delay >= CHORUS_MAX_DELAY_LENGTH) modulated_delay = CHORUS_MAX_DELAY_LENGTH - 1;

		// Set delay length
		chorus->delay_line.delay_length = (uint32_t)modulated_delay;

		// Process through delay line
		q15_t delayed_sample = Sound_Delay_Line(&chorus->delay_line, input[i]);

		// Store the output sample
		output[i] = (input[i] >> 1 ) + (delayed_sample >> 1 ) + out_device;
	}
}

// Initialize the chorus effect
ITCM_AREA_CODE void Effect_Chorus_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
Chorus_Effect_TypeDef *chorus = (Chorus_Effect_TypeDef *)effect->private_data;

	Sound_Delay_Line_Init(&chorus->delay_line, CHORUS_MAX_DELAY_LENGTH); // Initial delay length (100 ms)
    chorus->lfo_phase = 0;
}


ITCM_AREA_CODE void Effect_Chorus(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
Chorus_Effect_TypeDef *chorus = (Chorus_Effect_TypeDef *)effect->private_data;

	if (( effect->status & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
		chorus_process(chorus,effect->in_buf,effect->out_buf + start_sample,effect->out_device,CHORUS_BLOCK_SIZE);
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			effect->out_buf[i + start_sample]  = effect->in_buf[i]+effect->out_device;
	}

}
