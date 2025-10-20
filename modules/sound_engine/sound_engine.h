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

#define	NUMBER_OF_AUDIO_SAMPLES			512
#define	HALF_NUMBER_OF_AUDIO_SAMPLES	(NUMBER_OF_AUDIO_SAMPLES/2)
#define	DAC_AUDIO_SAMPLES_SIZE			NUMBER_OF_AUDIO_SAMPLES
#define	DAC_AUDIO_BUF_SIZE				NUMBER_OF_AUDIO_SAMPLES
#define	I2S_AUDIO_BUF_SIZE				NUMBER_OF_AUDIO_SAMPLES*2
#define	I2S_AUDIO_HALF_BUF_SIZE			NUMBER_OF_AUDIO_SAMPLES

#define	SOUND_BLOCK_SIZE				HALF_NUMBER_OF_AUDIO_SAMPLES

#define	SOUND_SOURCE_SYNTH				0
#define	SOUND_SOURCE_ANALOG				1

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
	uint8_t				i2s_handle;
	int16_t 			*codec_buf;
	uint16_t			out_device;				/* from previous module : for dac is 1 , for codec is 0 */
}PTR_Effect_TypeDef;

typedef struct {
	uint8_t				status;
	uint16_t			out_device;		/* for dac is 1 , for codec is 0 */
	int16_t				*out_buf;
	uint32_t 			*effect_s;		/* pointer to feffect */
	int16_t 			*codec_buf;
	float				sample_rate;
} SoundSource_TypeDef;

#define	SOUND_EFFECT_INITIALIZED		0x40
#define SOUND_EFFECT_ENABLED	 		0x80

#include	"Generators/synth.h"
#include	"Effects/vca.h"

extern 	PTR_Effect_TypeDef *Sound_Apply_Effect(uint32_t *effect);
extern	uint8_t				Sound_Insert_Effect(uint32_t *ext_source,uint32_t *new_effect);

#endif /* MODULES_SOUND_ENGINE_SOUND_ENGINE_H_ */
