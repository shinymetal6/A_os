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
 * drum_machine.h
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUM_MACHINE_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUM_MACHINE_H_

#define BLOCK_SIZE      64
#define NUM_VOICES      4
#define PATTERN_STEPS   16
#define MIXER_BUFFER_SIZE BLOCK_SIZE

// Drum IDs
#define DRUM_KICK       0
#define DRUM_SNARE      1
#define DRUM_HH         2
#define DRUM_CLAP       3

// Each voice: sample data, length, position, active flag
typedef struct {
    const int16_t	*sample;
    uint32_t 		length;
    uint32_t 		pos;
    float 			volume;	// 0.0f to 1.0f
    uint8_t 		playing;
} DrumVoice_TypeDef;

typedef struct {
	DrumVoice_TypeDef voices[NUM_VOICES];
	uint8_t		pattern[NUM_VOICES][PATTERN_STEPS];
	uint8_t 	current_step;
	uint32_t 	step_counter;
	uint32_t 	step_interval; //  30000 = ~120 BPM at 48kHz
} DrumMachine_TypeDef;

extern uint8_t 	Drum_Machine_Register(AUDIO_Source_TypeDef *drum_machine);
extern void 	Drum_Machine_audio(void);
extern void 	Drum_Machine_Update_sequencer(void);

#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUM_MACHINE_H_ */
