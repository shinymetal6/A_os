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
 * moog_filter.h
 *
 *  Created on: Apr 30, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_MOOG_FILTER_H_
#define MODULES_SOUND_EFFECTS_MOOG_FILTER_H_

#define MOOG_F_BLOCK_SIZE SOUND_BLOCK_SIZE          // Block size (number of samples per block)
#define MOOG_F_SAMPLE_RATE DEFAULT_SAMPLE_FREQUENCY       // Sample rate in Hz
#define MOOG_F_NUM_CUTOFF_STEPS 1024   // Number of steps in the lookup table

typedef struct {
	uint8_t lfo_enabled;
	float	cutoffFrequency;// = 1000.0f; // Initial cutoff frequency (1 kHz)
	float	resonance; // = 0.5f;          // Resonance (0.0 to 1.0)
	float	lfo_rate;						// Rate of the LFO [0 .. 1.0F] easy for user side

    /* Internals */
	float 	lfo_phase;						// Phase of the LFO
	float	lfo_increment;

	float 	cutoffLookupTable[MOOG_F_NUM_CUTOFF_STEPS];
	float 	stage1;
	float	stage2;
	float 	stage3;
	float 	stage4; // Filter stages
	float 	feedback; // Feedback signal
} MOOG_F_Effect_TypeDef;

#endif /* MODULES_SOUND_EFFECTS_MOOG_FILTER_H_ */
