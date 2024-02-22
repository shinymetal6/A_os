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
 * Project : bb1xx_743_00 
*/
/*
 * oscillator_core.h
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#ifndef STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATOR_CORE_H_
#define STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATOR_CORE_H_

#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#define	NUMOSCILLATORS	64
typedef struct _OscillatorsTypeDef
{
	uint32_t 				oscillator_age;
	uint8_t 				state;
	uint8_t 				midi_note;
	uint8_t 				waveform;
	int16_t					*wave;
	uint16_t 				current_phase;
	uint16_t 				delta_phase;
	float 					detune;
	uint8_t 				duty;
	float 					volume;
} OscillatorsTypeDef;

/* state */
#define	OSCILLATOR_ON					0x01
#define	OSCILLATOR_GO_OFF				0x02
#define	OSCILLATOR_GO_ON				0x04

#define	INT_PRECISION					256
#define	INT_PRECISION_SHIFT				(INT_PRECISION >> 5)
#define	MAX_DETUNE_PERCENT_F			2.0f
#define	SINE							0
#define	TRIANGLE						1
#define	SQUARE							2
#define	NOISE							3

#define	DAC_BIT							16
#define	DAC_RESOLUTION					(1<<DAC_BIT)
#define	WAVETABLE_SIZE					AUDIO_BUF_SIZE

#define	MIDI_NOTES						128

#define	VOICES							2

/*
#define	OSC_OFF							0
#define	OSC_GO_OFF						1
#define	OSC_ON							2
*/

#define	DUTY_SCALE	(256.0F/100.0F)

#define	INVALID_MIDI_NOTE				0x80

#define	MAX_SINEVAL	16363
#define	MIN_SINEVAL	-16363

enum ADSR_STATUS
{
	OSC_ATTACK_STATE,
	OSC_DECAY_STATE,
	OSC_SUSTAIN_STATE,
	OSC_RELEASE_STATE,
	ADSR_CLOSE_STATE
};

extern	void SetADSR_oscParams(uint32_t osc_number, uint8_t velocity );
extern	void InitOscillatorsTables(void);
extern	void RunOscillator32(void);

#endif //#ifdef SYNTH_ENGINE_ENABLE
#endif /* STM32H743_AUDIOPROCESSES_AUDIO_COMPONENTS_GENERATORS_OSCILLATOR_CORE_H_ */
