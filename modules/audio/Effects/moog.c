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
 * moog.c
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
#include "moog.h"

// Diode clipping function (Q15)
ITCM_AREA_CODE static q15_t diode_clip_q15(q15_t x)
{
    // Approximate tanh using a piecewise linear function
    if (x > 32767) return 32767; // Clamp to maximum Q15 value
    if (x < -32768) return -32768; // Clamp to minimum Q15 value

    // Simple approximation: y = x / (1 + |x|)
    q31_t abs_x = (x < 0) ? -x : x; // Absolute value of x
    q31_t denominator = (1 << 15) + abs_x; // Add 1.0 (scaled to Q15)
    q31_t result = ((q31_t)x << 15) / denominator; // Divide scaled x by denominator
    return (q15_t)(result >> 15); // Scale back to Q15
}

// Soft saturation function (Q15)
ITCM_AREA_CODE static q15_t soft_saturation_q15(q15_t x)
{
    if (x > 32767) return 32767; // Clamp to maximum Q15 value
    if (x < -32768) return -32768; // Clamp to minimum Q15 value

    // Approximate cubic polynomial for soft saturation
    q31_t x_scaled = (q31_t)x * x; // Square the input (Q15 * Q15 -> Q30)
    x_scaled >>= 15;               // Scale back to Q15
    q15_t result = x - (q15_t)(x_scaled * x / (32768L * 3)); // Subtract scaled cube
    return result;
}

// Apply Moog filter to a single sample (Q15)
ITCM_AREA_CODE static q15_t moog_filter_process_q15(MOOG_Effect_TypeDef	*moog,q15_t input_sample)
{
    // Precompute constants
    q15_t f = arm_sin_q15(moog->MOOG_Filter_Params->cutoff_freq); // Frequency coefficient (scaled sine wave)
    q15_t r = moog->MOOG_Filter_Params->resonance;                // Resonance coefficient

    // Nonlinear feedback term with diode clipping
    q15_t fb = diode_clip_q15(moog->MOOG_Filter_State->z4 - ((q31_t)input_sample * r >> 15));

    // State updates with soft saturation
    q15_t stage1 = soft_saturation_q15(moog->MOOG_Filter_State->z1 + ((q31_t)f * (input_sample - moog->MOOG_Filter_State->z1 + fb) >> 15));
    q15_t stage2 = soft_saturation_q15(moog->MOOG_Filter_State->z2 + ((q31_t)f * (stage1 - moog->MOOG_Filter_State->z2) >> 15));
    q15_t stage3 = soft_saturation_q15(moog->MOOG_Filter_State->z3 + ((q31_t)f * (stage2 - moog->MOOG_Filter_State->z3) >> 15));
    q15_t stage4 = soft_saturation_q15(moog->MOOG_Filter_State->z4 + ((q31_t)f * (stage3 - moog->MOOG_Filter_State->z4) >> 15));

    // Update state variables
    moog->MOOG_Filter_State->z1 = stage1;
    moog->MOOG_Filter_State->z2 = stage2;
    moog->MOOG_Filter_State->z3 = stage3;
    moog->MOOG_Filter_State->z4 = stage4;

    return stage4; // Output is the last stage
}

ITCM_AREA_CODE static void moog_init(MOOG_Effect_TypeDef *moog)
{
	moog->cutoff_freq = 0.1f;
	moog->resonance = 0.5f;
	moog->MOOG_Filter_Params->cutoff_freq = (q15_t)(moog->cutoff_freq * 32768.0f);
	moog->MOOG_Filter_Params->resonance = (q15_t)(moog->resonance * 32768.0f);
	moog->MOOG_Filter_State->z1 = moog->MOOG_Filter_State->z2 = moog->MOOG_Filter_State->z3 = moog->MOOG_Filter_State->z4 = 0;
}

ITCM_AREA_CODE void Do_Moog(int16_t *inputData, int16_t *outputData, uint8_t index)
{
MOOG_Effect_TypeDef	*moog = (MOOG_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;
	if (( moog->flags & EFFECT_INITIALIZED) == 0)
	{
		moog_init(moog);
		moog->flags |= EFFECT_INITIALIZED;
	}

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( moog->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
				outputData[i] = moog_filter_process_q15(moog,inputData[i]);
		else
				outputData[i] = inputData[i];
	}
}

ITCM_AREA_CODE void ParamsEffectSet_Moog(uint32_t *param_struct, uint8_t index)
{
MOOG_Effect_TypeDef	*moog = (MOOG_Effect_TypeDef *)Effects[index].private_data;
MOOG_ParamsChange_Effect_TypeDef	*params = (MOOG_ParamsChange_Effect_TypeDef *)param_struct;

	moog->cutoff_freq 	= params->cutoff_freq;
	moog->resonance 	= params->resonance;
	moog_init(moog);
}

#endif
