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
 * filter_lattice.h
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_FILTER_LATTICE_H_
#define MODULES_SOUND_ENGINE_EFFECTS_FILTER_LATTICE_H_

#include <stdint.h>
/*
						K1	K2	Effect
Mild phase shift		0.3 0.2	Subtle coloration
Strong all-pass 		0.8 0.6	Pronounced phase distortion
Reverb building block 	0.7 0.5 Used in Schroeder all-pass
*/
// ======================
// 2nd-Order Lattice
// ======================
typedef struct {
    float k1;           // Reflection coefficient 1 (|k| < 1.0)
    float k2;           // Reflection coefficient 2 (|k| < 1.0)
    float g0_delay;     // g0[n-1]
    float g1_delay;     // g1[n-1]
} lattice_2nd_t;

void lattice_2nd_init(lattice_2nd_t *filter, float k1, float k2);
void lattice_2nd_process(lattice_2nd_t *filter, const float *input, float *output, uint32_t size);

// ======================
// N-Stage Lattice
// ======================
#define LATTICE_MAX_STAGES 8

typedef struct {
    float k[LATTICE_MAX_STAGES];        // Reflection coefficients
    float g_delay[LATTICE_MAX_STAGES];  // Delay elements
    uint8_t stages;                     // Number of active stages (1-8)
} lattice_nstage_t;

void lattice_nstage_init(lattice_nstage_t *filter, const float *k_coeffs, uint8_t stages);
void lattice_nstage_process(lattice_nstage_t *filter, const float *input, float *output, uint32_t size);


#endif /* MODULES_SOUND_ENGINE_EFFECTS_FILTER_LATTICE_H_ */
