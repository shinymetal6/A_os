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
 * filter_feedback_comb.h
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_FILTER_FEEDBACK_COMB_H_
#define MODULES_SOUND_ENGINE_EFFECTS_FILTER_FEEDBACK_COMB_H_

#include <stdint.h>

 #define COMB_DELAY_SAMPLES  960   // For 50 Hz

#define COMB_FEEDBACK_DEFAULT_GAIN  0.85f // Must be < 1.0 (0.7–0.95 typical)
/*
 The delay line len defines the notch point, the feedback gain defines how deep the notch is
60 Hz 48000 / 60 = 800 -> 60, 120, 180, ... Hz
50 Hz 48000 / 50 = 960 -> 50, 100, 150, ... Hz
 */
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
	float 				delay_line[COMB_DELAY_SAMPLES];
	float				comb_feedback_gain;
	uint16_t 			comb_delay_samples;
	uint16_t 			delay_ptr;

} FEEDBACK_COMB_Effect_TypeDef;
#define FEEDBECK_COMB_UPDATE_PARAMS 	0x01           		// Number of cascaded biquad stages (order = 2 * NUM_BIQUADS)


#endif /* MODULES_SOUND_ENGINE_EFFECTS_FILTER_FEEDBACK_COMB_H_ */
