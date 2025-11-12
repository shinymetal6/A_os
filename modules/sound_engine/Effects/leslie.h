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
 * leslie.h
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_LESLIE_H_
#define MODULES_SOUND_ENGINE_EFFECTS_LESLIE_H_

#include <stdint.h>
#include <stdbool.h>

#define LESLIE_DELAY_LINE_SIZE 2048 // Size of the delay buffer (must be power of 2 for wrap-around optimization)

// Structure to hold leslie parameters and state
typedef struct {
    float delay_buffer[LESLIE_DELAY_LINE_SIZE]; // The delay line
    int write_index;              // Current write position in the delay buffer
    float read_position;          // Current read position (can be fractional)
    float max_delay_ms;           // Maximum delay in milliseconds (controls "spread")
    float lfo_rate_slow_rpm;      // Slow LFO rate (e.g., 6 RPM -> Hz)
    float lfo_rate_fast_rpm;      // Fast LFO rate (e.g., 360 RPM -> Hz)
    float lfo_depth;              // Depth of delay modulation (0.0 to 1.0)
    bool fast_mode;               // Flag to switch between slow/fast speeds
    float lfo_phase;              // Current phase of the LFO oscillator
    float sample_rate;            // Audio sample rate (e.g., 48000.0f)
    bool bypass;                  // Flag to bypass the effect
} Leslie;

// Function Prototypes
void Leslie_Init(Leslie *ls, float max_delay_ms, float slow_rpm, float fast_rpm, float depth, float sample_rate);
void Leslie_SetSpeed(Leslie *ls, bool fast);
void Leslie_SetBypass(Leslie *ls, bool bypass);
int32_t Leslie_ProcessSample(Leslie *ls, int32_t input_sample);

#endif /* MODULES_SOUND_ENGINE_EFFECTS_LESLIE_H_ */
