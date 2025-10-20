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
 * phaser.h
 *
 *  Created on: Apr 25, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_PHASER_H_
#define MODULES_SOUND_EFFECTS_PHASER_H_

#define PHASER_BUFFER_SIZE		SOUND_BLOCK_SIZE  	// 128 samples
#define PHASER_DEFAULT_LFO_RATE	2.0F	// LFO frequency in Hz
#define PHASER_DEFAULT_DEPTH	0.7F	// Depth of modulation
#define PHASER_DEFAULT_MIX		0.5F	// mix rate
#define PHASER_NUM_ALLPASS		6		// allpass stages, modifies depth

typedef struct
{
	/* effect header */
	uint32_t 		*pre_effect;
	uint32_t 		*next_effect;
	q15_t			*effect_in_buf;
	q15_t			*effect_out_buf;
	void 			(*effect)(uint32_t 	*effect_data);
	void 			(*effect_init)(uint32_t *effect_data);
	uint8_t			status;
	uint8_t			flags;
	/* effect data */
	uint16_t		*lfo_rate;						// Rate of the LFO [0 .. 1.0F] easy for user side
	uint16_t		*depth;							// Depth of the LFO
	uint16_t		*mix;
	float			sample_rate;
	/* internals */
	float 			f_lfo_rate;						// Rate of the LFO [0 .. 1.0F] easy for user side
	float 			f_depth;							// Depth of the LFO
	float 			f_mix;
	int32_t			allpass_number;					// Read position in the buffer
	float 			lfo_phase;						// Phase of the LFO
	float			lfo_increment;
	float 			depth_sum,depth_mul;							// Depth of the LFO
	float 			buffer[PHASER_BUFFER_SIZE];	// Circular buffer for delay line
	int32_t			write_pos;						// Write position in the buffer
	int32_t			read_pos;						// Read position in the buffer
}PHASER_Effect_TypeDef;

extern void Effect_Phaser(uint32_t *effect_s);
extern void Effect_Phaser_Init(uint32_t *effect_s);

#endif /* MODULES_SOUND_EFFECTS_PHASER_H_ */
