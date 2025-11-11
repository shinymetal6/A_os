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
 * bitcrusher.h
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_BITCRUSHER_H_
#define MODULES_SOUND_ENGINE_EFFECTS_BITCRUSHER_H_

#include <stdint.h>
#include <stdbool.h>

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
    uint8_t 			bit_depth;        // Target bit depth (e.g., 4 to 16)
    uint8_t 			downsample_factor; // Factor by which to reduce sample rate (e.g., 1=no change, 4 = 1/4 rate)
	uint32_t			time_start;
	uint32_t			effect_time;
	/* effect data */
    uint8_t 			i_bit_depth;        // Target bit depth (e.g., 4 to 16)
    uint8_t 			i_downsample_factor; // Factor by which to reduce sample rate (e.g., 1=no change, 4 = 1/4 rate)
    uint32_t 			sample_counter;  // Internal counter for downsampling
    float	 			last_sample;      // Store the last sample used for downsampling hold
    bool bypass;              // Flag to bypass the effect
}BITCRUSHER_Effect_TypeDef;

extern void Effect_BITCRUSHER_Init(uint32_t *effect_s);
extern void Effect_BITCRUSHER(uint32_t *effect_s);

// Function Prototypes
extern	void 	BitCrusher_SetBitDepth(BITCRUSHER_Effect_TypeDef *bitcrusher, uint8_t i_bit_depth);
extern	void 	BitCrusher_SetDownsampleFactor(BITCRUSHER_Effect_TypeDef *bitcrusher, uint8_t i_downsample_factor);
extern	void	Effect_BITCRUSHER_Init(uint32_t *effect_s);
extern	void	Effect_BITCRUSHER(uint32_t *effect_s);

#endif /* MODULES_SOUND_ENGINE_EFFECTS_BITCRUSHER_H_ */
