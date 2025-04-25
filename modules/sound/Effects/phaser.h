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
 * phaser.h
 *
 *  Created on: Apr 25, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_PHASER_H_
#define MODULES_SOUND_EFFECTS_PHASER_H_

#define PHASER_SAMPLE_RATE 			DEFAULT_SAMPLE_FREQUENCY      // Sampling rate in Hz
#define PHASER_BLOCK_SIZE 			SOUND_BLOCK_SIZE         // Number of samples processed per block
#define PHASER_NUM_STAGES 			4           // Number of all-pass filter stages
#define PHASER_LFO_FREQ 			1.0f          // LFO frequency in Hz
#define PHASER_FEEDBACK_GAIN_Q15 	16384 // Feedback gain (0.5 in Q15 format)
#define PHASER_FEEDBACK_GAIN	 	0.5F // Feedback gain (0.5 in Q15 format)

// All-pass filter state
typedef struct {
    q15_t prev_in;  // Previous input sample
    q15_t prev_out; // Previous output sample
} Phaser_AllPassFilter_TypeDef;

// Phaser state
typedef struct {
    float lfo_frequency;
    float feedback_gain;
    /* Internals */
    float lfo_phase;                 // LFO phase (0.0 to 1.0)
    float lfo_increment;             // LFO phase increment per sample
    q15_t feedback;                  // Feedback gain
    Phaser_AllPassFilter_TypeDef stages[PHASER_NUM_STAGES]; // Array of all-pass filter stages
} PHASER_Effect_TypeDef;

extern void Effect_Phaser(uint32_t *effect_s, uint32_t start_sample);
extern void Effect_Phaser_Init(uint32_t *effect_s);

#endif /* MODULES_SOUND_EFFECTS_PHASER_H_ */
