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
 * flanger.h
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_FLANGER_H_
#define MODULES_SOUND_ENGINE_EFFECTS_FLANGER_H_

typedef struct
{
	/* effect header */
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_effect;
	q15_t				*in_buf;
	q15_t				*out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint16_t			block_size;
	float				sample_rate;
	/* Here finishes the common area */
	uint16_t			*lfoFreq;
	uint16_t			*depth;
	uint16_t			*maxDelay;
	uint16_t			flanger_maxDelay;
	uint32_t			time_start;
	uint32_t			effect_time;
	/* effect data */
	float				phase;
	float				f_lfoFreq;
	float				f_lfoDepth;
	uint16_t			*offset;
}FLANGER_Effect_TypeDef;

#define FLANGER_MAX_FREQ_LFO		7.0F
#define FLANGER_MAX_DEPTH			100

extern void Effect_FLANGER_Init(uint32_t *effect_s);
extern void Effect_FLANGER(uint32_t *effect_s);

#endif /* MODULES_SOUND_ENGINE_EFFECTS_FLANGER_H_ */
