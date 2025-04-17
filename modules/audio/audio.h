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
#define	CONTROL_OSC_VCF_DLY				0x01
#define	CONTROL_FM						0x02
#define	CONTROL_VCA						0x04
#define	CONTROL_ROLLBACK2ADSR			0x08
#define	CONTROL_ADC_FLAG				0x10
#define	CONTROL_TICK_FLAG				0x20
#define	CONTROL_SYSTICK_FLAG			0x40
#define	CONTROL_MIDIRX_FLAG				0x80

#define	AUDIO_LEFT_CH					0
#define	AUDIO_RIGHT_CH					1

//#define AUDIO_BUF_SIZE 					256

#define	NUMBER_OF_AUDIO_SAMPLES			256
#define	HALF_NUMBER_OF_AUDIO_SAMPLES	(NUMBER_OF_AUDIO_SAMPLES/2)
#define DEFAULT_SAMPLE_FREQUENCY 		48000
#define I2S_AUDIO_BUF_SIZE 				(NUMBER_OF_AUDIO_SAMPLES*2)
#define	AUDIO_IS_MONO					0
#define	AUDIO_IS_STEREO					1
#define	OSCILLATORS						1

#include 	"effects.h"

#ifdef AUDIO_ENABLED
	#if defined(STM32H7xx_HAL_I2S_H) || defined(STM32H7xx_HAL_DAC_H)
		#define AUDIO_FAST_RAM		__attribute__((section(".dtcm_user_data"))) __attribute__ ((aligned (16)))
		#define DMA_NOCACHE_RAM		__attribute__((section(".dmaNoCache")))  	__attribute__ ((aligned (32)))
		#define	AUDIO_GENERATORS_ENABLED	1
	#endif
#endif

/*
#if defined(ARM_MATH_CM7) || defined (ARM_MATH_CM4) || defined (ARM_MATH_CM3) || defined (ARM_MATH_CM0) || defined (ARM_MATH_CM0PLUS)
#include 	"Effects/arm_math.h"
#include	"Effects/adsr.h"
#include	"Effects/biquad_s.h"
#include	"Effects/distortion.h"
#include	"Effects/flanger.h"
#include	"Effects/fft.h"
#include	"Effects/iir.h"
#include	"Effects/moog_filters.h"
#include	"Effects/passthrough.h"
#include	"Effects/phaser.h"
#include	"Effects/pitch_shift.h"
#include	"Effects/reverb.h"
#include	"Effects/tremolo.h"
#include	"Effects/vca_s.h"
#include	"Effects/vibrato.h"
#include	"Effects/wah.h"
#endif
*/
#include	"Effects/vca.h"
#include	"Effects/echo.h"
#include	"Effects/dummy.h"
#include	"Effects/reverb.h"
#include	"Effects/phaser.h"
#include	"Effects/fft.h"

#include	"Generators/noise.h"
#include	"Generators/oscillator_core.h"
#include	"Generators/oscillators.h"

extern	int16_t		oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
extern	int16_t		pipe_out[HALF_NUMBER_OF_AUDIO_SAMPLES];
extern	int16_t		audio_pipe[MAX_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];


#endif /* MODULES_AUDIO_AUDIO_H_ */
