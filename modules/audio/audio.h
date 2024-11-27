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
 * audio.h
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_AUDIO_H_
#define MODULES_AUDIO_AUDIO_H_

#ifdef STM32H7xx_HAL_I2S_H

typedef struct
{
	int16_t		channel[2];
}WaveLR_t;

typedef struct _AudioFlagsTypeDef
{
	uint8_t 	audio_flags;
	uint8_t 	control_flags;
	float		master_volume;

}AudioFlagsTypeDef;

/* audio_flags values */
#define	AUDIO_HALFBUFIN_FLAG		0x01
#define	AUDIO_HALFBUFOUT_FLAG		0x02
#define	AUDIO_OUT_READY_FLAG		0x10
#define	AUDIO_IN_READY_FLAG			0x20
#define	AUDIO_GENERATE_FLAG			0x80

/* control_flags values */
/*
#define	CONTROL_OSC_FROM_CV			0x01
#define	CONTROL_OSC_FROM_MIDI		0x02
*/
#define	CONTROL_OSC_VCF_DLY			0x01
#define	CONTROL_FM					0x02
#define	CONTROL_VCA					0x04
#define	CONTROL_ROLLBACK2ADSR		0x08
#define	CONTROL_ADC_FLAG			0x10
#define	CONTROL_TICK_FLAG			0x20
#define	CONTROL_SYSTICK_FLAG		0x40
#define	CONTROL_MIDIRX_FLAG			0x80

#define	AUDIO_LEFT_CH					0
#define	AUDIO_RIGHT_CH					1

#define AUDIO_BUF_SIZE 					1024

#define	NUMBER_OF_AUDIO_SAMPLES			AUDIO_BUF_SIZE
#define	HALF_NUMBER_OF_AUDIO_SAMPLES	(NUMBER_OF_AUDIO_SAMPLES/2)
#define SAMPLE_FREQUENCY 				44100

#include 	"effects.h"

#include 	"Effects/arm_math.h"

#include	"Effects/adsr.h"
#include	"Effects/biquad_s.h"
#include	"Effects/distortion.h"
#include	"Effects/echo.h"
#include	"Effects/flanger.h"
#include	"Effects/fft.h"
#include	"Effects/iir.h"
#include	"Effects/moog_filters.h"
#include	"Effects/passthrough.h"
#include	"Effects/phaser.h"
#include	"Effects/pitch_shift.h"
#include	"Effects/reverb.h"
#include	"Effects/tremolo.h"
#include	"Effects/vca.h"
#include	"Effects/vca_s.h"
#include	"Effects/vibrato.h"
#include	"Effects/wah.h"

#include	"Generators/noise.h"
#include	"Generators/oscillator_core.h"
#include	"Generators/oscillators.h"


extern	uint32_t *InitAudioBuffers(void);
extern	uint8_t StartAudioBuffers(int16_t* audio_in_buffer,int16_t* audio_out_buffer);
extern	void SetEffectMode(void);
extern	void SetGeneratorMode(void);
extern	void Vca( WaveLR_t *buffer_out,WaveLR_t *buffer_in,uint16_t	start);
extern	void SetMasterVolume(uint16_t volume);
extern	void EnableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity);
extern	void InitOscillators(void);
extern	void DisableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity);
extern	void EnableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity);
extern	void RunOscillator32(void);

#endif // #ifdef STM32H7xx_HAL_I2S_H

#endif /* MODULES_AUDIO_AUDIO_H_ */
