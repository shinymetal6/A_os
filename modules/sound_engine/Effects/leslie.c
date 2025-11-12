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
 * leslie.c
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

#include "../sound_engine.h"
#include "leslie.h"

#include <stdint.h>
#include <math.h> // For pow() if needed
#include <string.h> // For memset

// Helper function for linear interpolation
static float interpolate(float a, float b, float t) {
    return a + t * (b - a);
}

// Initialize the leslie structure
void Leslie_Init(Leslie *ls, float max_delay_ms, float slow_rpm, float fast_rpm, float depth, float sample_rate) {
    memset(ls, 0, sizeof(Leslie)); // Clear all fields

    ls->max_delay_ms = max_delay_ms;
    ls->lfo_rate_slow_rpm = slow_rpm / 60.0f; // Convert RPM to Hz
    ls->lfo_rate_fast_rpm = fast_rpm / 60.0f;
    ls->lfo_depth = depth;
    ls->sample_rate = sample_rate;
    ls->fast_mode = false;
    ls->bypass = false;

    // Initialize delay buffer
    memset(ls->delay_buffer, 0, sizeof(ls->delay_buffer));
}

// Set speed (slow/fast)
void Leslie_SetSpeed(Leslie *ls, bool fast) {
    ls->fast_mode = fast;
}

// Enable/Disable bypass
void Leslie_SetBypass(Leslie *ls, bool bypass) {
    ls->bypass = bypass;
    if (bypass) {
        // Optionally clear delay buffer when bypassing
        memset(ls->delay_buffer, 0, sizeof(ls->delay_buffer));
        ls->write_index = 0;
        ls->read_position = 0.0f;
    }
}

// Process a single audio sample
int32_t Leslie_ProcessSample(Leslie *ls, int32_t input_sample) {
    if (ls->bypass) {
        return input_sample; // Return original sample if bypassed
    }

    // --- Get current LFO value (sine wave) ---
    float lfo_freq = ls->fast_mode ? ls->lfo_rate_fast_rpm : ls->lfo_rate_slow_rpm;
    ls->lfo_phase += 2.0f * M_PI * lfo_freq / ls->sample_rate;
    if (ls->lfo_phase >= 2.0f * M_PI) {
        ls->lfo_phase -= 2.0f * M_PI; // Wrap phase
    }
    float lfo_val = sinf(ls->lfo_phase); // Range: -1 to 1

    // --- Calculate Modulated Delay (in samples) ---
    float max_delay_samples = (ls->max_delay_ms / 1000.0f) * ls->sample_rate;
    float modulated_delay = max_delay_samples * ls->lfo_depth * (lfo_val + 1.0f) / 2.0f; // Range: 0 to max_depth * max_delay_samples

    // --- Write Input Sample to Delay Buffer ---
    ls->delay_buffer[ls->write_index] = (float)input_sample;
    ls->write_index = (ls->write_index + 1) % LESLIE_DELAY_LINE_SIZE; // Increment and wrap

    // --- Calculate Read Position ---
    float read_index_float = (float)ls->write_index - modulated_delay - 1.0f; // -1 for delay, adjust if needed
    // Wrap the read index if it goes negative or beyond buffer size
    while (read_index_float < 0.0f) {
        read_index_float += LESLIE_DELAY_LINE_SIZE;
    }
    while (read_index_float >= LESLIE_DELAY_LINE_SIZE) {
        read_index_float -= LESLIE_DELAY_LINE_SIZE;
    }

    // --- Read from Delay Buffer (Linear Interpolation) ---
    int read_index_int = (int)read_index_float;
    float fraction = read_index_float - (float)read_index_int;
    int next_index = (read_index_int + 1) % LESLIE_DELAY_LINE_SIZE;

    float delayed_sample = interpolate(ls->delay_buffer[read_index_int], ls->delay_buffer[next_index], fraction);

    // --- Optional: Add some feedback for a more intense effect ---
    // float feedback_gain = 0.1f; // Be careful with this to avoid instability
    // delayed_sample += feedback_gain * ls->delay_buffer[ls->write_index]; // Write index is now updated

    // Convert back to int32_t and return
    // Clamp output to prevent overflow if needed
    int32_t output_sample = (int32_t)delayed_sample;
    if (output_sample > INT32_MAX) output_sample = INT32_MAX;
    if (output_sample < INT32_MIN) output_sample = INT32_MIN;

    return output_sample;
}

#endif // #ifdef SOUND_ENGINE_ENABLED
