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
 *  Created on: Apr 18, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_IIR_H_
#define MODULES_AUDIO_EFFECTS_IIR_H_

#define IIR_BLOCK_SIZE 		HALF_NUMBER_OF_AUDIO_SAMPLES         // Number of samples processed per block
#define IIR_NUM_STAGES 		2           // Number of biquad stages (order = 2 * NUM_STAGES)
#define IIR_SAMPLE_RATE		DEFAULT_SAMPLE_FREQUENCY

typedef enum {
    FILTER_TYPE_LOW_PASS,
    FILTER_TYPE_HIGH_PASS,
    FILTER_TYPE_BAND_PASS,
    FILTER_TYPE_BAND_STOP
} IIR_Effect_FilterType_TypeDef;

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float32_t 		cutoff_freq;
	float32_t 		sample_rate;
	float32_t 		q_factor;
	IIR_Effect_FilterType_TypeDef 		filter_type;
	q15_t			iir_coeffs[IIR_NUM_STAGES * 5]; // Coefficients for the filter (b0, b1, b2, a1, a2)
	q15_t 			iir_state[IIR_NUM_STAGES * 4];  // State variables for the filter
	arm_biquad_casd_df1_inst_q15 iir_filter;
}IIR_Effect_TypeDef;

#endif /* MODULES_AUDIO_EFFECTS_IIR_H_ */
