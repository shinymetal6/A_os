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
 *  Created on: Apr 7, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_PHASER_H_
#define MODULES_AUDIO_EFFECTS_PHASER_H_

#define MAX_DELAY 1024       // Maximum delay length for all-pass filters
#define PHASER_SAMPLE_RATE DEFAULT_SAMPLE_FREQUENCY    // Sample rate in Hz

#define NUM_STAGES 6         // Number of all-pass filter stages
#define LFO_FREQ 1.0f        // LFO frequency in Hz
#define DEPTH 0.5f           // Modulation depth (0.0 to 1.0)
#define FEEDBACK 0.7f        // Feedback amount (0.0 to 1.0)

typedef struct {
    float buffer[MAX_DELAY]; // Circular buffer for delay line
    int write_index;         // Write index for circular buffer
    int delay_samples;       // Delay time in samples
} AllPassFilter_TypeDef;

// Phaser structure combining multiple all-pass filters and LFO
typedef struct {
	AllPassFilter_TypeDef 	filters[NUM_STAGES];
    float 					lfo_phase;         // Current phase of the LFO
    float 					feedback_buffer;   // Feedback buffer
} Phaser_TypeDef;

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float 			lfo_freq,depth,feedback;
	Phaser_TypeDef	*phaser;
}PHASER_Effect_TypeDef;

#define	PHASER_EFFECT_INITIALIZED	0x01

extern void Do_Phaser(int16_t *inputData, int16_t *outputData, uint8_t index);


#endif /* MODULES_AUDIO_EFFECTS_PHASER_H_ */
