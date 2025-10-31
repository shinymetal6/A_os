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
 * vca.h
 *
 *  Created on: Apr 24, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_VCA_H_
#define MODULES_SOUND_EFFECTS_VCA_H_
#ifdef SOUND_ENGINE_ENABLED

typedef struct
{
	/* effect header */
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_effect;
	q15_t				*in_buf;
	q15_t				*out_buf;
	int16_t				*device_out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint16_t			block_size;
	float				sample_rate;
	uint8_t				in_device;
	uint8_t				out_device;
	uint8_t				channel_in,channel_out;
	uint32_t			time_start;
	uint32_t			effect_time;
	/* effect data */
	uint16_t			*amplitude;
	uint16_t			*offset;
}VCA_Effect_TypeDef;

extern void Effect_VCA_Init(uint32_t *effect_s);
extern void Effect_VCA(uint32_t *effect_s);

#endif // #ifdef SOUND_ENGINE_ENABLED
#endif /* MODULES_SOUND_EFFECTS_VCA_H_ */
