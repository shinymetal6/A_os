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
 * iir.c
 *
 *  Created on: Apr 18, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
//#include "../../kernel/kernel_opt.h"

#include "../audio.h"
#ifdef AUDIO_GENERATORS_ENABLED
#include "../effects.h"
#include "arm_math.h"
#include "iir.h"

// Function to design a programmable IIR filter
ITCM_AREA_CODE static void design_iir_filter_q15(IIR_Effect_TypeDef	*iir)
{
    float32_t omega = 2.0f * 3.14159265358979323846f * iir->cutoff_freq / iir->sample_rate;
    float32_t alpha = sinf(omega) / (2.0f * iir->q_factor);
    float32_t cos_omega = cosf(omega);

    float32_t a0 = 1.0f + alpha;

    float32_t b0, b1, b2, a1, a2;

    switch (iir->filter_type) {
        case FILTER_TYPE_LOW_PASS:
            b0 = (1.0f - cos_omega) / (2.0f * a0);
            b1 = (1.0f - cos_omega) / a0;
            b2 = b0;
            a1 = (-2.0f * cos_omega) / a0;
            a2 = (1.0f - alpha) / a0;
            break;

        case FILTER_TYPE_HIGH_PASS:
            b0 = (1.0f + cos_omega) / (2.0f * a0);
            b1 = -(1.0f + cos_omega) / a0;
            b2 = b0;
            a1 = (-2.0f * cos_omega) / a0;
            a2 = (1.0f - alpha) / a0;
            break;

        case FILTER_TYPE_BAND_PASS:
            b0 = alpha / a0;
            b1 = 0.0f;
            b2 = -alpha / a0;
            a1 = (-2.0f * cos_omega) / a0;
            a2 = (1.0f - alpha) / a0;
            break;

        case FILTER_TYPE_BAND_STOP:
            b0 = 1.0f / a0;
            b1 = (-2.0f * cos_omega) / a0;
            b2 = 1.0f / a0;
            a1 = (-2.0f * cos_omega) / a0;
            a2 = (1.0f - alpha) / a0;
            break;

        default:
            return;
    }

    // Convert coefficients to Q15 format
    iir->iir_coeffs[0] = (q15_t)(b0 * 32768.0f); // Scale by 2^15
    iir->iir_coeffs[1] = (q15_t)(b1 * 32768.0f);
    iir->iir_coeffs[2] = (q15_t)(b2 * 32768.0f);
    iir->iir_coeffs[3] = (q15_t)(a1 * 32768.0f);
    iir->iir_coeffs[4] = (q15_t)(a2 * 32768.0f);
}

// Initialize the IIR filter
ITCM_AREA_CODE static void init_iir_filter_q15(IIR_Effect_TypeDef	*iir)
{
    // Design the filter coefficients
    design_iir_filter_q15(iir);

    // Initialize the IIR filter instance
    arm_biquad_cascade_df1_init_q15(&iir->iir_filter, IIR_NUM_STAGES, iir->iir_coeffs, iir->iir_state, 15); // Post-shift = 15
}

ITCM_AREA_CODE static void iir_init(IIR_Effect_TypeDef	*iir)
{
	iir->sample_rate = IIR_SAMPLE_RATE;
	init_iir_filter_q15(iir);
}

ITCM_AREA_CODE void Do_Iir(int16_t *inputData, int16_t *outputData, uint8_t index)
{
IIR_Effect_TypeDef	*iir = (IIR_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;
	if (( iir->flags & EFFECT_INITIALIZED) == 0)
	{
		iir_init(iir);
		iir->flags |= EFFECT_INITIALIZED;
	}

	if (( iir->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
	{
	    arm_biquad_cascade_df1_q15(&iir->iir_filter, inputData, outputData, IIR_BLOCK_SIZE);
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

#endif
