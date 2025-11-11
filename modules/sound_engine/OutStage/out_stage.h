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
 * out_stage.h
 *
 *  Created on: Nov 4, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_OUTSTAGE_OUT_STAGE_H_
#define MODULES_SOUND_ENGINE_OUTSTAGE_OUT_STAGE_H_

typedef struct
{
	/* Common with effects header */
	uint8_t				status;
	uint8_t				flags;
	q15_t				*in_buf;
	q15_t				*out_buf;
	uint16_t			block_size;
	uint8_t				out_device;
	uint8_t				channel;
	void				(*OutFunc)(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel);
}AUDIO_Dest_TypeDef;
/* mixer_config */
#define	OUT_SYNTH_FROM_LEFT		0x01
#define	OUT_SYNTH_FROM_RIGHT	0x02
#define	OUT_I2S_FROM_LEFT		0x04
#define	OUT_I2S_FROM_RIGHT		0x08

extern uint8_t OutStage_Register(AUDIO_Dest_TypeDef *out_stage);

#endif /* MODULES_SOUND_ENGINE_OUTSTAGE_OUT_STAGE_H_ */
