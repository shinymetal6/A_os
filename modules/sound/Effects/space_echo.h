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
 * space_echo.h
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_SPACE_ECHO_H_
#define MODULES_SOUND_EFFECTS_SPACE_ECHO_H_

#ifndef SPACE_ECHO_H
#define SPACE_ECHO_H

#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define ECHO_MAX_DELAY_MS 800   // Max delay time
#define ECHO_BUFFER_SIZE (ECHO_MAX_DELAY_MS * 48) // 48 kHz → ~38,400 samples

typedef struct
{
	uint8_t		status;
	uint8_t		flags;
	// Delay buffer (mono for now; duplicate for stereo)
    float buffer[ECHO_BUFFER_SIZE];
    uint32_t write_ptr;

    // Parameters
    float delay_time_ms;        // 50–800 ms
    float feedback;             // 0.0–0.8
    float lfo_rate;             // 0.5–3 Hz
    float lfo_depth_ms;         // 0–10 ms
    float cutoff;               // LPF on feedback (200–5000 Hz)

    // LFO state
    float lfo_phase;

    // Moog filter state (for feedback path)
    float g, k;
    float y1, y2, y3, y4;

    float sample_rate;
} SPACE_ECHO_Effect_TypeDef;

void space_echo_init(SPACE_ECHO_Effect_TypeDef *echo, float sample_rate);
void space_echo_set_params(SPACE_ECHO_Effect_TypeDef *echo, float delay_ms, float feedback, float lfo_rate, float lfo_depth, float lpf_cutoff);
float space_echo_process(SPACE_ECHO_Effect_TypeDef *echo, float input);

#endif

#endif /* MODULES_SOUND_EFFECTS_SPACE_ECHO_H_ */
