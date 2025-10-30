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
 * schroeder_reverb.h
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_SCHROEDER_REVERB_H_
#define MODULES_SOUND_ENGINE_EFFECTS_SCHROEDER_REVERB_H_

#include <stdint.h>

// Delay lengths (samples) for 48 kHz — chosen to be mutually prime
#define AP1_DELAY  1051  // ~22 ms
#define AP2_DELAY  337   // ~7 ms
#define AP3_DELAY  113   // ~2.4 ms
#define AP4_DELAY  37    // ~0.8 ms

// Reflection coefficients (|k| < 1.0)


typedef struct {
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
	/* effect data */
	float 				ap1_buffer[AP1_DELAY];
	float 				ap2_buffer[AP2_DELAY];
	float 				ap3_buffer[AP3_DELAY];
	float 				ap4_buffer[AP4_DELAY];
	uint16_t 			ap1_r, ap1_w;
	uint16_t 			ap2_r, ap2_w;
	uint16_t 			ap3_r, ap3_w;
	uint16_t 			ap4_r, ap4_w;
	uint16_t 			delay_ptr;
	float 				K1,K2,K3,K4;

} SCHROEDER_REVERB_Effect_TypeDef;
#define SCHROEDER_REVERB_UPDATE_PARAMS 	0x01           		// Number of cascaded biquad stages (order = 2 * NUM_BIQUADS)


#endif /* MODULES_SOUND_ENGINE_EFFECTS_SCHROEDER_REVERB_H_ */
