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
 * harmonizer.h
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_SFX_HARMONIZER_H_
#define MODULES_SOUND_ENGINE_SFX_HARMONIZER_H_

#include "../arm_math.h"
/*
 *  Harmony Modes
Mode			Interval		Use Case
HARMONY_4TH		+5 semitones	Blues, rock
HARMONY_5TH		+7 semitones	Power chords
HARMONY_OCTAVE	+12 semitones	Thick bass or lead
HARMONY_DUAL	+5th + octave	Rich, chorus-like effect
 */

#define BLOCK_SIZE      64
//#define GRAIN_SIZE      128
#define MAX_VOICES      3   // Dry + up to two harmony voices

extern float32_t audioIn[BLOCK_SIZE];
extern float32_t audioOut[BLOCK_SIZE];

// Parameters
extern float32_t mix;           // Wet/dry mix (0.0 = dry, 1.0 = wet)
extern uint8_t harmony_mode;    // 0=off, 1=4th, 2=5th, 3=octave, 4=dual

// Harmony modes
#define HARMONY_OFF       0
#define HARMONY_4TH       1
#define HARMONY_5TH       2
#define HARMONY_OCTAVE    3
#define HARMONY_DUAL      4

// Structure for each harmony voice
typedef struct {
    float32_t phase;        // Read position in grain
    float32_t crossfade;    // For smooth envelope
} HARMONIZER_VoiceState_TypeDef;

typedef struct {
	/* effect header */
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_effect;
	q15_t				*in_buf;
	q15_t				*out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint16_t			block_size;
	float				sample_rate;
	/* Here finishes the common area */
	uint32_t			time_start;
	uint32_t			effect_time;
	/* effect data */
	float				grain_buffer[GRAIN_SIZE]; // Grain buffer for recording incoming audio
	uint16_t 			write_index;
	int 				grain_ready;
	float	 			mix; // default 0.8f;
	uint8_t 			harmony_mode; // default HARMONY_5TH;
	HARMONIZER_VoiceState_TypeDef voices[MAX_VOICES];
}HARMONIZER_Effect_TypeDef;


#endif /* MODULES_SOUND_ENGINE_SFX_HARMONIZER_H_ */
