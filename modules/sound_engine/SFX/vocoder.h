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
 * vocoder.h
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_SFX_VOCODER_H_
#define MODULES_SOUND_ENGINE_SFX_VOCODER_H_

#define NUM_BANDS       10

extern float audioInMod[BLOCK_SIZE];   // Modulator (voice)
extern float audioInCar[BLOCK_SIZE];   // Carrier (synth)
extern float audioOut[BLOCK_SIZE];

// Biquad filter structure
typedef struct {
    float b0, b1, b2;
    float a1, a2;
    float x1, x2;  // Previous inputs
    float y1, y2;  // Previous outputs
}VOCODER_BiquadFilter_TypeDef;

typedef struct {
	/* effect header */
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_effect;
	q15_t				*modulation_buf;
	q15_t				*out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint16_t			block_size;
	float				sample_rate;
	/* Here finishes the common area */
	q15_t				*carrier_buf;
	uint16_t			*attack_time_Msec;	// In millisecond : 10 ms -> 10
	uint16_t			*release_time_Msec;	// In millisecond : 100 ms -> 100

	uint32_t			time_start;
	uint32_t			effect_time;
	/* effect data */
	float 				attack_time_sec;   	// In second : 10 ms -> 0.01F
	float 				release_time_sec;	// In second : 100 ms -> 0.1F
	float				attack_coef;
	float				release_coef;
	float 				envelopes[NUM_BANDS];
	VOCODER_BiquadFilter_TypeDef bpf_filters[NUM_BANDS];

}VOCODER_Effect_TypeDef;

#endif /* MODULES_SOUND_ENGINE_SFX_VOCODER_H_ */
