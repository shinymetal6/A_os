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
 * sound_engine.h
 *
 *  Created on: Oct 20, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_SOUND_ENGINE_H_
#define MODULES_SOUND_ENGINE_SOUND_ENGINE_H_

#include "main.h"
#include "arm_math.h"

#include	"Generators/audio_sources.h"

#if defined USER_SAMPLE_RATE
	#define DEFAULT_SAMPLE_FREQUENCY 		USER_SAMPLE_RATE
#else
	#define DEFAULT_SAMPLE_FREQUENCY 		96000
#endif

#define	DEFAULT_NUMBER_OF_AUDIO_SAMPLES			512
#define	DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES	(DEFAULT_NUMBER_OF_AUDIO_SAMPLES/2)

/* source_type */
#define		SOUND_SOURCE_IS_SYNTH		0
#define		SOUND_SOURCE_IS_I2S_IN		1
/* out_device */
#define		SOURCE_TO_INTERNAL_BUF		0
#define		SOURCE_TO_DAC_OUT			1
#define		SOURCE_TO_I2S_OUT			2
/* channel_in */
#define		AUDIO_SOURCE_LEFT			0
#define		AUDIO_SOURCE_RIGHT			1
/* channel_out */
#define		AUDIO_DESTINATION_LEFT		0
#define		AUDIO_DESTINATION_RIGHT		1

#define	SOUND_EFFECT_INITIALIZED		0x40
#define SOUND_EFFECT_ENABLED	 		0x80

extern	float					Sound_Sample_Frequency;

#include	"OutStage/out_stage.h"
#include	"Generators/synth.h"
#include	"Generators/i2s_in.h"
#include	"Effects/effects.h"


extern	AUDIO_Source_TypeDef	*AudioSourceLeft;
extern	AUDIO_Source_TypeDef	*AudioSourceRight;
extern	AUDIO_Dest_TypeDef		*AudioDestLeft;
extern	AUDIO_Dest_TypeDef		*AudioDestRight;

extern	uint32_t			audio_pipe_time;

extern void to_i2sout(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel);
extern void to_dacout(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel);
extern void to_intbuf(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel);

extern 	float 				fast_tanh(float x);
extern	uint8_t				Sound_Insert_Effect(uint32_t *ext_source,uint32_t *new_effect);
extern	uint8_t				Sound_Remove_Effect(uint32_t *ext_source,uint32_t *remove_effect);
extern	uint8_t				Sound_Change_Sample_Frequency(uint32_t new_sample_frequency);
extern void 				Do_Audio(uint32_t start_sample);

#endif /* MODULES_SOUND_ENGINE_SOUND_ENGINE_H_ */
