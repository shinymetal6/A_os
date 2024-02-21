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
 * oscillators.h
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#ifndef STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATORS_H_
#define STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATORS_H_

#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

typedef struct _OscillatorsFlagsTypeDef
{
	uint8_t 	oscillator_flags;
	uint8_t		osc_waves[4];
	uint8_t 	osc_duty[4];
	uint8_t 	osc_volume[4];	// range 0..10
	uint8_t 	osc_duty_percent[4];
}OscillatorsFlagsTypeDef;

/* oscillator_flags */
#define	OSC_VOLUME_PENDING			0x01
#define	OSC_DETUNE_PENDING			0x02
#define	OSC_DUTY_PENDING			0x04
#define	OSC_WAVE_PENDING			0x08
#define	OSC_FM_PENDING				0x10
#define	OSC_TUNE_PENDING			0x20
//NOT USED							0x40
#define	OSC_SRCFLAG					0x80	/* 0 is from Cv , 1 is from MIDI */


extern	void InitOscillators(void);
extern	void EnableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity);
extern	void DisableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity);
//#define OSCILLATORS_RAM


#endif //#ifdef SYNTH_ENGINE_ENABLE


#endif /* STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATORS_H_ */
