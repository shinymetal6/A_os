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
 * granular_pitch_shifter.h
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_SFX_GRANULAR_PITCH_SHIFTER_H_
#define MODULES_SOUND_ENGINE_SFX_GRANULAR_PITCH_SHIFTER_H_

#include "../arm_math.h"
/*
pitch_factor	Effect
1.0f			Original pitch
0.5f			One octave down
1.5f			Perfect fifth up
2.0f			One octave up

mix = 1.0f	Fully effected
mix = 0.5f	50% dry/wet blend

*/
#define BLOCK_SIZE      64

typedef struct {
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
	uint32_t			time_start;
	uint32_t			effect_time;
	/* effect data */
	float				grain_buffer[GRAIN_SIZE]; // Grain buffer for recording incoming audio
	uint16_t 			write_index;
	int 				grain_ready;
	float	 			pitch_factor;  	// 0.5 = octave down, 2.0 = octave up
	float	 			mix; 			// default 0.8f;
	float	 			play_phase;
	float	 			crossfade;
}GRANULAR_PITCH_SHIFTER_Effect_TypeDef;

#endif /* MODULES_SOUND_ENGINE_SFX_GRANULAR_PITCH_SHIFTER_H_ */
