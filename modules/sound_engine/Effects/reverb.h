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
 * reverb.h
 *
 *  Created on: Apr 25, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_REVERB_H_
#define MODULES_SOUND_EFFECTS_REVERB_H_

#define REVERB_SAMPLE_RATE 			DEFAULT_SAMPLE_FREQUENCY      // Sampling rate in Hz
#define REVERB_BLOCK_SIZE 			SOUND_BLOCK_SIZE         // Number of samples processed per block
#define REVERB_NUM_COMB_FILTERS 	4     // Number of comb filters
#define REVERB_NUM_ALLPASS_FILTERS  4  // Number of all-pass filters
#define REVERB_MAX_DELAY_LENGTH 	DEFAULT_SAMPLE_FREQUENCY/4 // Maximum delay length (1 second at 48 kHz)

#define REVERB_DEFAULT_FBK_GAIN 	0.7F
#define REVERB_DEFAULT_DRY_MIX	 	0.5F
#define REVERB_DEFAULT_WET_MIX	 	0.5F

// Delay line structure
typedef struct {
	q15_t buffer[REVERB_MAX_DELAY_LENGTH]; // Circular buffer for delay line
    uint32_t write_index;           // Write index for circular buffer
    uint32_t delay_length;          // Delay length in samples
}  Reverb_DelayLine_TypeDef;

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
    uint16_t *feedback_gain;
    uint16_t *mix;
    /* Internals */
    float f_feedback_gain;
    float f_dry_mix;
    float f_wet_mix;

    Reverb_DelayLine_TypeDef *comb_filters; // Comb filters
    Reverb_DelayLine_TypeDef *allpass_filters; // All-pass filters
    q15_t feedback_gain_q15; // Feedback gain for comb filters
}REVERB_Effect_TypeDef;

/*
Reverb_DelayLine_TypeDef comb_filters[REVERB_NUM_COMB_FILTERS]; // Comb filters
Reverb_DelayLine_TypeDef allpass_filters[REVERB_NUM_ALLPASS_FILTERS]; // All-pass filters
*/

extern void Effect_Reverb_Init(uint32_t *effect_s);
extern void Effect_Reverb(uint32_t *effect_s);

#endif /* MODULES_SOUND_EFFECTS_REVERB_H_ */
