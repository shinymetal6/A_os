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
#define REVERB_NUM_ALLPASS_FILTERS 2  // Number of all-pass filters
#define REVERB_MAX_DELAY_LENGTH 	DEFAULT_SAMPLE_FREQUENCY // Maximum delay length (1 second at 44.1 kHz)

#define REVERB_DEFAULT_FBK_GAIN 	0.7F
#define REVERB_DEFAULT_DRY_MIX	 	0.5F
#define REVERB_DEFAULT_WET_MIX	 	0.5F

// Delay line structure
typedef struct {
//    q15_t buffer[REVERB_MAX_DELAY_LENGTH]; // Circular buffer for delay line
    q15_t *buffer; // Circular buffer for delay line
    uint32_t write_index;           // Write index for circular buffer
    uint32_t delay_length;          // Delay length in samples
} Reverb_DelayLine_TypeDef;

typedef struct
{
    float feedback_gain;
    float dry_mix;
    float wet_mix;
    /* Internals */
    Reverb_DelayLine_TypeDef comb_filters[REVERB_NUM_COMB_FILTERS]; // Comb filters
    Reverb_DelayLine_TypeDef allpass_filters[REVERB_NUM_ALLPASS_FILTERS]; // All-pass filters
    q15_t feedback_gain_q15; // Feedback gain for comb filters
    q15_t mix_dry_q15;       // Dry signal mix (Q15 format)
    q15_t mix_wet_q15;       // Wet signal mix (Q15 format)
}REVERB_Effect_TypeDef;

extern void Effect_Reverb_Init(uint32_t *effect_s);
extern void Effect_Reverb(uint32_t *effect_s, uint32_t start_sample);


#endif /* MODULES_SOUND_EFFECTS_REVERB_H_ */
