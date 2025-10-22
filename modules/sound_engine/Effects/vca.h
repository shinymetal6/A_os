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
	uint32_t 			*pre_effect;
	uint32_t 			*next_effect;
	q15_t				*effect_in_buf;
	q15_t				*effect_out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint8_t				status;
	uint8_t				flags;
	uint16_t			synth_block_size;
	uint16_t			out_device;		/* for dac is 1 , for codec is 0 */
	/* effect data */
	uint16_t			*amplitude;
	uint16_t			*offset;
}VCA_Effect_TypeDef;

extern void Effect_VCA_Init(uint32_t *effect_s);
extern void Effect_VCA(uint32_t *effect_s);

#endif // #ifdef SOUND_ENGINE_ENABLED
#endif /* MODULES_SOUND_EFFECTS_VCA_H_ */
