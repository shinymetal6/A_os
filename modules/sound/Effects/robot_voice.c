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
 * robot_voice.c
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"
#ifdef SOUND_ENABLED
#ifdef ENABLE_TESTING

#include "robot_voice.h"

// Simple 2-pole bandpass filter (bilinear transform)
static void bp_filter_update_coeffs(ROBOT_VOICE_Effect_TypeDef *rv, float *a1, float *a2, float *b0, float *b1) {
    float w0 = 2.0f * M_PI * rv->bp_freq / rv->sample_rate;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float alpha = sin_w0 / (2.0f * rv->bp_q);

    float a0 = 1.0f + alpha;
    *b0 = (0.5f * sin_w0) / a0;
    *b1 = 0.0f;
    *a1 = (-2.0f * cos_w0) / a0;
    *a2 = (1.0f - alpha) / a0;
}

void robot_init(ROBOT_VOICE_Effect_TypeDef *rv, float fs) {
    rv->sample_rate = fs;
    rv->ring_phase = 0.0f;
    robot_set_params(rv, 600.0f, 1, 8.0f, 1200.0f, 1.2f); // default: square wave, 8-bit, 1.2kHz BP
}

void robot_set_params(ROBOT_VOICE_Effect_TypeDef *rv, float ring_freq, uint8_t ring_wave, float bit_depth, float bp_freq, float bp_q) {
    rv->ring_freq = (ring_freq < 100.0f) ? 100.0f : (ring_freq > 2000.0f ? 2000.0f : ring_freq);
    rv->ring_wave = ring_wave;
    rv->bit_depth = (bit_depth < 4.0f) ? 4.0f : (bit_depth > 16.0f ? 16.0f : bit_depth);
    rv->bp_freq = (bp_freq < 200.0f) ? 200.0f : (bp_freq > 4000.0f ? 4000.0f : bp_freq);
    rv->bp_q = (bp_q < 0.3f) ? 0.3f : (bp_q > 3.0f ? 3.0f : bp_q);

    rv->bp_x1 = rv->bp_x2 = 0.0f;
    rv->bp_y1 = rv->bp_y2 = 0.0f;
}

float robot_process(ROBOT_VOICE_Effect_TypeDef *rv, float input) {
    const float fs = rv->sample_rate;

    // --- 1. Ring Modulation ---
    float carrier;
    rv->ring_phase += rv->ring_freq / fs;
    if (rv->ring_phase >= 1.0f) rv->ring_phase -= 1.0f;

    if (rv->ring_wave == 0) {
        carrier = sinf(2.0f * M_PI * rv->ring_phase);
    } else {
        carrier = (rv->ring_phase < 0.5f) ? 1.0f : -1.0f; // square
    }

    float modulated = input * carrier;

    // --- 2. Bit Crushing ---
    float scale = powf(2.0f, rv->bit_depth - 1.0f); // e.g., 8-bit → 128
    float crushed = roundf(modulated * scale) / scale;

    // --- 3. Bandpass Filter (emulates vocal tract) ---
    float a1, a2, b0, b1;
    bp_filter_update_coeffs(rv, &a1, &a2, &b0, &b1);

    float bp_out = b0 * crushed + b1 * rv->bp_x1 - a1 * rv->bp_y1 - a2 * rv->bp_y2;

    // Update filter state
    rv->bp_x2 = rv->bp_x1;
    rv->bp_x1 = crushed;
    rv->bp_y2 = rv->bp_y1;
    rv->bp_y1 = bp_out;

    return bp_out;
}
#endif // #ifdef ENABLE_TESTING

#endif // #ifdef SOUND_ENABLED

