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
 * space_echo.c
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

#include "space_echo.h"

static float read_delay_line(float *buffer, uint32_t size, uint32_t wptr, float delay_samples) {
    float rptr_f = (float)wptr - delay_samples;
    if (rptr_f < 0) rptr_f += (float)size;

    uint32_t rptr_i = (uint32_t)rptr_f;
    float frac = rptr_f - (float)rptr_i;

    uint32_t rptr_next = (rptr_i + 1) % size;

    // Linear interpolation
    return buffer[rptr_i] + frac * (buffer[rptr_next] - buffer[rptr_i]);
}

void space_echo_init(SPACE_ECHO_Effect_TypeDef *echo, float fs) {
    for (int i = 0; i < ECHO_BUFFER_SIZE; i++) echo->buffer[i] = 0.0f;
    echo->write_ptr = 0;
    echo->sample_rate = fs;
    echo->lfo_phase = 0.0f;
    space_echo_set_params(echo, 300.0f, 0.5f, 1.2f, 3.0f, 1200.0f);
}

void space_echo_set_params(SPACE_ECHO_Effect_TypeDef *echo, float delay_ms, float fb, float lfo_rate, float lfo_depth, float lpf_cutoff) {
    if (delay_ms < 20.0f) delay_ms = 20.0f;
    if (delay_ms > ECHO_MAX_DELAY_MS) delay_ms = ECHO_MAX_DELAY_MS;
    echo->delay_time_ms = delay_ms;

    echo->feedback = (fb < 0.0f) ? 0.0f : (fb > 0.85f ? 0.85f : fb);
    echo->lfo_rate = lfo_rate;
    echo->lfo_depth_ms = lfo_depth;
    echo->cutoff = lpf_cutoff;

    // Precompute Moog g for feedback LPF
    float g = tanf(M_PI * lpf_cutoff / echo->sample_rate);
    if (g > 10.0f) g = 10.0f;
    echo->g = g;
    echo->k = 4.0f * 0.7f; // fixed resonance for warmth
}

float space_echo_process(SPACE_ECHO_Effect_TypeDef *echo, float input) {
    const float fs = echo->sample_rate;

    // --- LFO for delay modulation (tape wow) ---
    echo->lfo_phase += 2.0f * M_PI * echo->lfo_rate / fs;
    if (echo->lfo_phase >= 2.0f * M_PI) echo->lfo_phase -= 2.0f * M_PI;

    float lfo_val = sinf(echo->lfo_phase); // [-1, 1]
    float mod_delay_ms = echo->delay_time_ms + lfo_val * echo->lfo_depth_ms;
    if (mod_delay_ms < 20.0f) mod_delay_ms = 20.0f;

    float delay_samples = mod_delay_ms * fs / 1000.0f;

    // --- Read delayed signal ---
    float delay_out = read_delay_line(echo->buffer, ECHO_BUFFER_SIZE, echo->write_ptr, delay_samples);

    // --- Apply low-pass filter to feedback path (simulate tape HF loss) ---
    float g = echo->g;
    float k = echo->k;
    float u = (delay_out - k * echo->y4) / (1.0f + g * (1.0f + g * (1.0f + g * (1.0f + g))));
    float stage = tanhf(u + g * echo->y1); echo->y1 = stage;
    stage = tanhf(stage + g * echo->y2);   echo->y2 = stage;
    stage = tanhf(stage + g * echo->y3);   echo->y3 = stage;
    stage = tanhf(stage + g * echo->y4);   echo->y4 = stage;
    float filtered_feedback = echo->y4;

    // --- Write input + feedback into delay line ---
    float write_val = input + filtered_feedback * echo->feedback;
    echo->buffer[echo->write_ptr] = write_val;

    // --- Advance pointer ---
    echo->write_ptr = (echo->write_ptr + 1) % ECHO_BUFFER_SIZE;

    // --- Output = dry + wet (adjust mix as needed) ---
    return input + delay_out * 0.7f; // 70% wet
}
#endif // #ifdef ENABLE_TESTING

#endif // #ifdef SOUND_ENABLED
