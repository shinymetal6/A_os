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
 * sound.h
 *
 *  Created on: Apr 19, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_SOUND_H_
#define MODULES_SOUND_SOUND_H_

#include <arm_math.h>

#define	NUMBER_OF_AUDIO_SAMPLES			256
#define	HALF_NUMBER_OF_AUDIO_SAMPLES	(NUMBER_OF_AUDIO_SAMPLES/2)
#define	SOUND_BLOCK_SIZE				HALF_NUMBER_OF_AUDIO_SAMPLES
#define DEFAULT_SAMPLE_FREQUENCY 		48000


/* macro for ease the writing */

#define	OBUF effect->out_buf[i + start_sample]
#define	IBUF effect->in_buf[i] + effect->out_device

/* END macro for ease the writing */

typedef struct {
	uint8_t				status;
	uint32_t 			*pre_effect_s;
	uint32_t 			*next_effect_s;
	q15_t				*in_buf;
	q15_t				*out_buf;
	uint16_t			out_device;				/* from previous module : for dac is 32768 , for codec is 0 */
	void 				(*effect)(uint32_t 	*effect_data , uint32_t start_sample);
	void 				(*effect_init)(uint32_t *effect_data);
	void 				(*effect_params_set)(void);
	uint32_t			*private_data;
} Effect_TypeDef;
#define SOUND_EFFECT_ENABLED	 		0x80
#define	SOUND_EFFECT_INITIALIZED		0x40

#include	"Synth/synth.h"
#include	"Effects/passthrough.h"
#include	"Effects/vca.h"
#include	"Effects/phaser.h"
#include	"Effects/reverb.h"
#include	"Effects/chorus.h"
#include	"Effects/echo.h"

extern uint8_t 	Sound_Insert_Effect(Effect_TypeDef *effect);
extern void 	Sound_Apply_Effect(Effect_TypeDef *effect,uint32_t start_sample);

#endif /* MODULES_SOUND_SOUND_H_ */
