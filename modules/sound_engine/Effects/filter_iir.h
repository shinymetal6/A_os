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
 * filter_iir.h
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_FILTER_IIR_H_
#define MODULES_SOUND_ENGINE_EFFECTS_FILTER_IIR_H_

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
	uint16_t			*cutoffFrequency;
	uint16_t			*bandwidth;
	FilterType			filterType;
    /* Internals */
	float 				f_cutoffFrequency;// = 1000.0f : cutoff frequency @1 kHz , center frequency for bw filters
	float 				f_bandwidth;// = 500.0f;        // Bandwidth for band-pass and notch filters
    BiquadFilter 		biquads[IIR_NUM_BIQUADS];
    BiquadFilter 		new_biquads[IIR_NUM_BIQUADS];
} IIR_Effect_TypeDef;
#define IIR_UPDATE_PARAMS 	0x01           		// Number of cascaded biquad stages (order = 2 * NUM_BIQUADS)

extern void Effect_IIR(uint32_t *effect_s);
extern void Effect_IIR_Init(uint32_t *effect_s);
extern void Effect_IIR_UpdateParams(IIR_Effect_TypeDef *iir );


#endif /* MODULES_SOUND_ENGINE_EFFECTS_FILTER_IIR_H_ */
