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
 * filter_lattice.c
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "filter_lattice.h"
#include <string.h>

// ======================
// 2nd-Order Implementation
// ======================
void lattice_2nd_init(lattice_2nd_t *filter, float k1, float k2)
{
    filter->k1 = k1;
    filter->k2 = k2;
    filter->g0_delay = 0.0f;
    filter->g1_delay = 0.0f;
}

void lattice_2nd_process(lattice_2nd_t *filter, const float *input, float *output, uint32_t size)
{
    float k1 = filter->k1;
    float k2 = filter->k2;
    float g0_delay = filter->g0_delay;
    float g1_delay = filter->g1_delay;

    for (uint32_t i = 0; i < size; i++) {
        float x = input[i];

        // Stage 1
        float f0 = x;
        float f1 = f0 + k1 * g0_delay;
        float g1_now = -k1 * f0 + g0_delay;

        // Stage 2 (all-pass: only g2 is needed)
        // float f2 = f1 + k2 * g1_delay;  // ← UNUSED - REMOVE
        float g2 = -k2 * f1 + g1_delay;

        output[i] = g2;

        // Update delays
        g0_delay = f0;
        g1_delay = g1_now;
    }

    filter->g0_delay = g0_delay;
    filter->g1_delay = g1_delay;
}

// ======================
// N-Stage Implementation
// ======================
void lattice_nstage_init(lattice_nstage_t *filter, const float *k_coeffs, uint8_t stages)
{
    if (stages > LATTICE_MAX_STAGES) {
        stages = LATTICE_MAX_STAGES;
    }

    filter->stages = stages;
    memcpy(filter->k, k_coeffs, stages * sizeof(float));
    memset(filter->g_delay, 0, LATTICE_MAX_STAGES * sizeof(float));
}

void lattice_nstage_process(lattice_nstage_t *filter, const float *input, float *output, uint32_t size)
{
    const uint8_t stages = filter->stages;
    float k[LATTICE_MAX_STAGES];
    float g_delay[LATTICE_MAX_STAGES];

    // Copy to local variables for speed
    memcpy(k, filter->k, stages * sizeof(float));
    memcpy(g_delay, filter->g_delay, stages * sizeof(float));

    for (uint32_t i = 0; i < size; i++) {
        float f_prev = input[i];
        float g_prev = f_prev; // g0 = f0 for all-pass

        // Process each stage
        for (uint8_t stage = 0; stage < stages; stage++) {
            float f_curr = f_prev + k[stage] * g_delay[stage];
            float g_curr = -k[stage] * f_prev + g_delay[stage];

            // Store g[n-1] for next sample
            g_delay[stage] = g_prev;

            f_prev = f_curr;
            g_prev = g_curr;
        }

        output[i] = g_prev;
    }

    // Save final state
    memcpy(filter->g_delay, g_delay, stages * sizeof(float));
}

#endif // #ifdef SOUND_ENGINE_ENABLED
