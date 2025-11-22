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
 * synth.h
 *
 *  Created on: Apr 23, 2025
 *      Author: fil
 */

#ifndef SRC_SYNTH_H_
#define SRC_SYNTH_H_

#ifdef SOUND_ENGINE_ENABLED
#include "audio_sources.h"

#define	SYNTH_WAVETABLE_256		256
#define	SYNTH_WAVETABLE_1024	1024
#define	SYNTH_MIDI_NOTES		128

#define	SYNTH_CHANNELS			2

#include	"audio_sources.h"

extern void 	Synth_Process_Block(uint32_t *the_synt);
extern uint8_t 	Synth_Register(AUDIO_Source_TypeDef *synth);
extern uint8_t	Synth_Start(AUDIO_Source_TypeDef *synth);
extern uint8_t 	Synth_Stop(AUDIO_Source_TypeDef *synth);

extern void NoteOn(uint8_t channel,uint8_t note, uint8_t velocity);
extern void NoteOff(uint8_t channel,uint8_t note);
extern void AllNoteOFF(void);

#endif // #ifdef SOUND_ENGINE_ENABLED
#endif /* SRC_SYNTH_H_ */
