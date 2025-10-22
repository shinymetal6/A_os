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
 * iir.h
 *
 *  Created on: Apr 30, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_IIR_H_
#define MODULES_SOUND_EFFECTS_IIR_H_

#define IIR_BLOCK_SIZE 		SOUND_BLOCK_SIZE	// Block size (number of samples per block)
#define IIR_NUM_BIQUADS 	4           		// Number of cascaded biquad stages (order = 2 * NUM_BIQUADS)

typedef enum {
    FILTER_TYPE_LOW_PASS,
    FILTER_TYPE_HIGH_PASS,
    FILTER_TYPE_BAND_PASS,
    FILTER_TYPE_NOTCH
} FilterType;

typedef struct {
    float b0, b1, b2; // Feedforward coefficients
    float a1, a2;     // Feedback coefficients
    float x1, x2;     // Past input values
    float y1, y2;     // Past output values
} BiquadFilter;

typedef struct {
	float 		cutoffFrequency;// = 1000.0f : cutoff frequency @1 kHz , center frequency for bw filters
	float 		bandwidth;// = 500.0f;        // Bandwidth for band-pass and notch filters
	FilterType	filterType;
} IIR_Effect_Params_TypeDef;

typedef struct {
	/* effect header */
	uint32_t 	*pre_effect;
	uint32_t 	*next_effect;
	q15_t		*effect_in_buf;
	q15_t		*effect_out_buf;
	void 		(*effect)(uint32_t 	*effect_data);
	void 		(*effect_init)(uint32_t *effect_data);
	uint8_t		status;
	uint8_t		flags;
	uint16_t	synth_block_size;
	uint16_t	out_device;		/* for dac is 1 , for codec is 0 */
	/* effect data */
	float		sample_rate;
	FilterType	filterType;
    /* Internals */
	float 		cutoffFrequency;// = 1000.0f : cutoff frequency @1 kHz , center frequency for bw filters
	float 		bandwidth;// = 500.0f;        // Bandwidth for band-pass and notch filters
    BiquadFilter biquads[IIR_NUM_BIQUADS];
} IIR_Effect_TypeDef;
#define IIR_UPDATE_PARAMS 	0x01           		// Number of cascaded biquad stages (order = 2 * NUM_BIQUADS)

extern void Effect_IIR(uint32_t *effect_s);
extern void Effect_IIR_Init(uint32_t *effect_s);
extern void Effect_IIR_SetParams(uint32_t *effect_s,uint32_t *params );


#endif /* MODULES_SOUND_EFFECTS_IIR_H_ */
