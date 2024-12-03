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
 * effects.h
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_H_
#define MODULES_AUDIO_EFFECTS_H_

#define	MAX_EFFECTS					16
#define	MAX_PARAMS					8

typedef struct
{
	void 		(*effect)(int16_t* inputData, int16_t* outputData,uint8_t index);
	int16_t		*pipe_in,*pipe_out;
	uint32_t	*private_data;
	uint32_t	effect_index;
} Effects_TypeDef;


#define	EFFECT_NOSPACELEFT	 255
#define	EFFECT_ENABLED 		0x80

extern	Effects_TypeDef		Effects[MAX_EFFECTS];

extern	uint32_t effect_insert(void (*do_effect),uint32_t *private_data,int16_t *dac_buffer);
extern	void effects_apply(uint8_t full_flag,int16_t *dac_buffer);


#endif /* MODULES_AUDIO_EFFECTS_H_ */
