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

#if defined USER_SAMPLE_RATE
	#define DEFAULT_SAMPLE_FREQUENCY 		USER_SAMPLE_RATE
#else
	#define DEFAULT_SAMPLE_FREQUENCY 		96000
#endif

/*
#define	DAC_AUDIO_SAMPLES_SIZE			NUMBER_OF_AUDIO_SAMPLES
#define	DAC_AUDIO_BUF_SIZE				NUMBER_OF_AUDIO_SAMPLES
#define	I2S_AUDIO_BUF_SIZE				NUMBER_OF_AUDIO_SAMPLES*2
#define	I2S_AUDIO_HALF_BUF_SIZE			NUMBER_OF_AUDIO_SAMPLES
*/
//#define	SOUND_BLOCK_SIZE				HALF_NUMBER_OF_AUDIO_SAMPLES
#define	DEFAULT_NUMBER_OF_AUDIO_SAMPLES			512
#define	DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES	(DEFAULT_NUMBER_OF_AUDIO_SAMPLES/2)

/* source_type */
#define		SOURCE_IS_SYNTH		0
#define		SYNTH_IS_I2S_IN		1

typedef struct
{
	uint32_t 			*pre_effect;
	uint32_t 			*next_effect;
	q15_t				*effect_in_buf;
	q15_t				*effect_out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint8_t				status;
	uint8_t				flags;
	uint16_t			synth_block_size;
	uint16_t			out_device;				/* from previous module : for dac is 1 , for codec is 0 */
	/* Here finishes the common area */
	int16_t 			*codec_buf;
	uint8_t				i2s_handle;
}PTR_Effect_TypeDef;

#ifdef ASDF
typedef struct {
	uint8_t				status;
	uint16_t			out_device;		/* for dac is 1 , for codec is 0 */
	int16_t				*out_buf;
	uint32_t 			*effect_s;		/* pointer to feffect */
	int16_t 			*codec_buf;
	float				sample_rate;
} SoundSource_TypeDef;
#endif

#define	SOUND_EFFECT_INITIALIZED		0x40
#define SOUND_EFFECT_ENABLED	 		0x80

extern	float				Sound_Sample_Frequency;
#define	AudioSource_TypeDef	Synth_TypeDef

#include	"Generators/synth.h"
#include	"Generators/i2s_in.h"
#include	"Effects/vca.h"

extern void synth_to_i2s_out(uint8_t synth_number,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples);
extern void synth_to_dac_out(uint8_t synth_number,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples);
extern void i2sin_to_i2sout(uint8_t synth_number,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples);

extern 	PTR_Effect_TypeDef *Sound_Apply_Effect(uint32_t *effect);
extern	uint8_t				Sound_Insert_Effect(uint32_t *ext_source,uint32_t *new_effect);
extern	uint8_t				Sound_Remove_Effect(uint32_t *ext_source,uint32_t *remove_effect);
extern	uint8_t				Sound_Change_Sample_Frequency(uint32_t new_sample_frequency);
extern void 				Do_Audio(uint32_t start_sample);

#endif /* MODULES_SOUND_ENGINE_SOUND_ENGINE_H_ */
