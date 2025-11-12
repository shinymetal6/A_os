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
 * granular_pitch_shifter.c
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

#include "../sound_engine.h"
#include "granular_pitch_shifter.h"
#include <string.h>
extern const float hanning_window[GRAIN_SIZE];

// Grain buffer
/*
static float32_t grain_buffer[GRAIN_SIZE];
static uint16_t write_index = 0;
static int grain_ready = 0;

// Playback state
static float32_t play_phase = 0.0f;     // Current read position in grain
static float32_t crossfade = 0.0f;      // For smooth grain transitions

extern float32_t hanning_window[];

// Global parameters
float32_t pitch_factor = 1.0f;
float32_t mix = 0.8f;
*/

ITCM_AREA_CODE static q15_t granular_pitch_shift_effect(GRANULAR_PITCH_SHIFTER_Effect_TypeDef *g_pitch_shifter, float input)
{
float	out_sample = 0.0f;
float	output;

	// === RECORD INTO GRAIN BUFFER ===
	g_pitch_shifter->grain_buffer[g_pitch_shifter->write_index] = input;
	g_pitch_shifter->write_index++;

	// Mark grain as ready when full
	if (g_pitch_shifter->write_index >= GRAIN_SIZE && !g_pitch_shifter->grain_ready) {
		g_pitch_shifter->grain_ready = 1;
		g_pitch_shifter->write_index = 0;  // Reset for next grain
	}

	// === PLAYBACK ONLY IF GRAIN IS READY ===
	if (g_pitch_shifter->grain_ready) {
		// Interpolate playback position
		uint16_t idx0 = (uint16_t)g_pitch_shifter->play_phase;
		uint16_t idx1 = (idx0 + 1) % GRAIN_SIZE;
		float32_t frac = g_pitch_shifter->play_phase - idx0;

		// Linear interpolation
		float32_t read_sample = g_pitch_shifter->grain_buffer[idx0] * (1.0f - frac) +
				g_pitch_shifter->grain_buffer[idx1] * frac;

		// Apply Hanning window
		float32_t windowed = read_sample * hanning_window[idx0];

		// Crossfade envelope for smooth looping
		g_pitch_shifter->crossfade += 1.0f / GRAIN_SIZE;
		if (g_pitch_shifter->crossfade > 1.0f)
			g_pitch_shifter->crossfade = 0.0f;
		float32_t fade = 0.5f * (1.0f - arm_cos_f32(2.0f * 3.14159265358979323846f * g_pitch_shifter->crossfade));

		out_sample = windowed * fade;
	}

	// Update playback phase based on pitch factor
	g_pitch_shifter->play_phase += 1.0f / g_pitch_shifter->pitch_factor;
	if (g_pitch_shifter->play_phase >= GRAIN_SIZE) {
		g_pitch_shifter->play_phase -= GRAIN_SIZE;
		g_pitch_shifter->crossfade = 0.0f;  // Reset fade at new cycle
	}

	// === Mix Dry and Wet Signals ===
	output = input * (1.0f - g_pitch_shifter->mix) + out_sample * g_pitch_shifter->mix;
	return __FLOAT_2_Q15(output);
}

ITCM_AREA_CODE void Effect_GRANULAR_PITCH_SHIFTER_Init(uint32_t *effect_s)
{
GRANULAR_PITCH_SHIFTER_Effect_TypeDef *g_pitch_shifter = (GRANULAR_PITCH_SHIFTER_Effect_TypeDef *)effect_s;
	if (( g_pitch_shifter->in_buf == NULL) || ( g_pitch_shifter->out_buf == NULL))
		return;
	memset(g_pitch_shifter->grain_buffer, 0, sizeof(g_pitch_shifter->grain_buffer));
    g_pitch_shifter->write_index = 0;
    g_pitch_shifter->grain_ready = 0;
    g_pitch_shifter->play_phase = 0.0f;
    g_pitch_shifter->crossfade = 0.0f;
    g_pitch_shifter->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_GRANULAR_PITCH_SHIFTER(uint32_t *effect_s)
{
uint32_t	i;
GRANULAR_PITCH_SHIFTER_Effect_TypeDef *g_pitch_shifter = (GRANULAR_PITCH_SHIFTER_Effect_TypeDef *)effect_s;

	if ((( g_pitch_shifter->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( g_pitch_shifter == NULL ))
		return;
	g_pitch_shifter->time_start = DWT->CYCCNT;

	if (( g_pitch_shifter->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		for ( i=0;i<g_pitch_shifter->block_size;i++)
			g_pitch_shifter->out_buf[i] = (q15_t ) granular_pitch_shift_effect(g_pitch_shifter,__Q15_2_FLOAT(g_pitch_shifter->in_buf[i]));
	}
	else
	{
		for ( i=0;i<g_pitch_shifter->block_size;i++)
			g_pitch_shifter->out_buf[i]  = g_pitch_shifter->in_buf[i];
	}
	g_pitch_shifter->effect_time = (DWT->CYCCNT - g_pitch_shifter->time_start) / (HSI_CLOCK / 1000000);
}

#endif // #ifdef SOUND_ENGINE_ENABLED
