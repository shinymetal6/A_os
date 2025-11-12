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
 * harmonizer.c
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

#include "../sound_engine.h"
#include "harmonizer.h"
#include <string.h>

//static HARMONIZER_VoiceState_TypeDef voices[MAX_VOICES];

// Precomputed semitone ratios (equal temperament)
static const float semitone_ratio[13] = {
    1.0000f, 1.0595f, 1.1225f, 1.1892f, 1.2599f,
    1.3348f, 1.4142f, 1.4983f, 1.5874f, 1.6818f,
    1.7818f, 1.8877f, 2.0000f
};

// Hanning window lookup table (generated for 128 points)
extern const float hanning_window[GRAIN_SIZE];

// Global parameters
/*
float mix = 0.8f;
uint8_t harmony_mode = HARMONY_5TH;
*/


ITCM_AREA_CODE static q15_t harmonizer_effect(HARMONIZER_Effect_TypeDef *harmonizer, float input)
{
float		wet_sum = 0.0f;
float		output;

	// === RECORD INTO GRAIN BUFFER ===
	harmonizer->grain_buffer[harmonizer->write_index] = input;
	harmonizer->write_index = (harmonizer->write_index + 1) % GRAIN_SIZE;

	// Mark grain as ready when it wraps around
	if (harmonizer->write_index == 0) {
		harmonizer->grain_ready = 1;
	}

	// Only process harmonies if we have a full grain
	if (!harmonizer->grain_ready)
	{
		output = input;
		return __FLOAT_2_Q15(output);
	}

	// === VOICE 1: Perfect 5th (+7 semitones) ===
	if (harmonizer->harmony_mode >= HARMONY_5TH)
	{
		float ratio = semitone_ratio[7];  // Perfect fifth

		uint16_t idx0 = (uint16_t)harmonizer->voices[1].phase;
		uint16_t idx1 = (idx0 + 1) % GRAIN_SIZE;
		float frac = harmonizer->voices[1].phase - idx0;

		float sample = harmonizer->grain_buffer[idx0] * (1.0f - frac) +
				harmonizer->grain_buffer[idx1] * frac;
		sample *= hanning_window[idx0];  // Apply window

		// Crossfade envelope
		float fade = 0.5f * (1.0f - arm_cos_f32(2.0f * 3.14159265358979323846f * harmonizer->voices[1].crossfade));
		wet_sum += sample * fade * 0.8f;  // Slightly lower level

		harmonizer->voices[1].phase += 1.0f / ratio;
		if (harmonizer->voices[1].phase >= GRAIN_SIZE) {
			harmonizer->voices[1].phase -= GRAIN_SIZE;
			harmonizer->voices[1].crossfade = 0.0f;
		} else {
			harmonizer->voices[1].crossfade += 1.0f / GRAIN_SIZE;
		}
	}

	// === VOICE 2: Octave Up (+12 semitones) – Only in DUAL mode ===
	if (harmony_mode == HARMONY_DUAL) {
		float ratio = semitone_ratio[12];

		uint16_t idx0 = (uint16_t)harmonizer->voices[2].phase;
		uint16_t idx1 = (idx0 + 1) % GRAIN_SIZE;
		float frac = harmonizer->voices[2].phase - idx0;

		float sample = harmonizer->grain_buffer[idx0] * (1.0f - frac) +
				harmonizer->grain_buffer[idx1] * frac;
		sample *= hanning_window[idx0];

		float fade = 0.5f * (1.0f - arm_cos_f32(2.0f * PI * harmonizer->voices[2].crossfade));
		wet_sum += sample * fade * 0.7f;

		harmonizer->voices[2].phase += 1.0f / ratio;
		if (harmonizer->voices[2].phase >= GRAIN_SIZE) {
			harmonizer->voices[2].phase -= GRAIN_SIZE;
			harmonizer->voices[2].crossfade = 0.0f;
		} else {
			harmonizer->voices[2].crossfade += 1.0f / GRAIN_SIZE;
		}
	}

	// === VOICE 0: Base Pitch (optional detune or unison can go here) ===
	// Currently just uses original signal as dry

	// === FINAL OUTPUT: Dry + Wet Mix ===
	output = input * (1.0f - harmonizer->mix) + wet_sum * harmonizer->mix;
	return __FLOAT_2_Q15(output);

}

ITCM_AREA_CODE void Effect_HARMONIZER_Init(uint32_t *effect_s)
{
HARMONIZER_Effect_TypeDef *harmonizer = (HARMONIZER_Effect_TypeDef *)effect_s;
	if (( harmonizer->in_buf == NULL) || ( harmonizer->out_buf == NULL))
		return;
	harmonizer->mix = 0.8F;
	harmonizer->harmony_mode = HARMONY_5TH;
	harmonizer->grain_ready = 0;
	harmonizer->write_index = 0;
    memset(harmonizer->grain_buffer, 0, sizeof(harmonizer->grain_buffer));

    for (int i = 0; i < MAX_VOICES; i++)
    {
    	harmonizer->voices[i].phase = (float)(i * 17);  // Stagger start positions
    	harmonizer->voices[i].crossfade = 0.0f;
    }
    harmonizer->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_HARMONIZER(uint32_t *effect_s)
{
uint32_t	i;
HARMONIZER_Effect_TypeDef *harmonizer = (HARMONIZER_Effect_TypeDef *)effect_s;

	if ((( harmonizer->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( harmonizer == NULL ))
		return;
	harmonizer->time_start = DWT->CYCCNT;
	if (( harmonizer->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		for ( i=0;i<harmonizer->block_size;i++)
			harmonizer->out_buf[i] = (q15_t ) harmonizer_effect(harmonizer,__Q15_2_FLOAT(harmonizer->in_buf[i]));
	}
	else
	{
		for ( i=0;i<harmonizer->block_size;i++)
			harmonizer->out_buf[i]  = harmonizer->in_buf[i];
	}
	harmonizer->effect_time = (DWT->CYCCNT - harmonizer->time_start) / (HSI_CLOCK / 1000000);
}
#endif // #ifdef SOUND_ENABLED

