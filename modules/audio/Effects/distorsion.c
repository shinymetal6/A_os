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
 * distorsion.c
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
#include "distorsion.h"

// Soft clipping function (Q15)
ITCM_AREA_CODE static q15_t soft_clip_q15(q15_t x)
{
    if (x > 32767)
    	return 32767; // Clamp to maximum Q15 value
    if (x < -32768)
    	return -32768; // Clamp to minimum Q15 value

    // Approximate cubic polynomial for soft clipping
    q31_t x_scaled = (q31_t)x * x; // Square the input (Q15 * Q15 -> Q30)
    x_scaled >>= 15;               // Scale back to Q15
    q15_t result = x - (q15_t)(x_scaled * x / (32768L * 3)); // Subtract scaled cube
    return result;
}

// Hard clipping function (Q15)
ITCM_AREA_CODE static q15_t hard_clip_q15(q15_t x)
{
    if (x > 32767)
    	return 32767; // Clamp to maximum Q15 value
    if (x < -32768)
    	return -32768; // Clamp to minimum Q15 value
    return x;
}

// Wave shaping function (tanh approximation, Q15)
ITCM_AREA_CODE static q15_t wave_shape_q15(q15_t x)
{
    // Approximate tanh using a piecewise linear function
    if (x > 32767)
    	return 32767; // Clamp to maximum Q15 value
    if (x < -32768)
    	return -32768; // Clamp to minimum Q15 value

    // Simple approximation: y = x / (1 + |x|)
    q31_t abs_x = (x < 0) ? -x : x; // Absolute value of x
    q31_t denominator = (1 << 15) + abs_x; // Add 1.0 (scaled to Q15)
    q31_t result = ((q31_t)x << 15) / denominator; // Divide scaled x by denominator
    return (q15_t)(result >> 15); // Scale back to Q15
}

// Distortion effect using CMSIS-DSP
ITCM_AREA_CODE static void distortion_process_q15(DISTORSION_Effect_TypeDef	*distorsion,q15_t *input, q15_t *output, int buffer_size)
{
    // Apply pre-gain using CMSIS-DSP scaling
    arm_scale_q15(input, distorsion->gain, 0, distorsion->temp, buffer_size);
    // Apply distortion based on type
    for (int i = 0; i < buffer_size; i++)
    {
        switch (distorsion->distorsion_type)
        {
            case SOFT: // Soft clipping
            	distorsion->temp[i] = soft_clip_q15(distorsion->temp[i]);
                break;
            case HARD: // Hard clipping
            	distorsion->temp[i] = hard_clip_q15(distorsion->temp[i]);
                break;
            case WAVE_SHAPE: // Wave shaping
            	distorsion->temp[i] = wave_shape_q15(distorsion->temp[i]);
                break;
            default:
            	distorsion->temp[i] = input[i]; // No distortion
                break;
        }
    }

    // Mix dry and wet signals using CMSIS-DSP vector operations

    arm_scale_q15(distorsion->temp, distorsion->mix, 0,distorsion->wet, buffer_size);          // Scale wet signal
    arm_scale_q15(input, (1 << 15) - distorsion->mix, 0,output, buffer_size); // Scale dry signal
    arm_add_q15(output, distorsion->wet, output, buffer_size);       // Add dry and wet signals
}

ITCM_AREA_CODE static void distorsion_init(DISTORSION_Effect_TypeDef	*distorsion)
{
	distorsion->gain = DISTORSION_PRE_GAIN;
	distorsion->mix = DISTORSION_MIX;
	distorsion->distorsion_type = WAVE_SHAPE;
}

ITCM_AREA_CODE void Do_Distorsion(int16_t *inputData, int16_t *outputData, uint8_t index)
{
DISTORSION_Effect_TypeDef	*distorsion = (DISTORSION_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;
	if (( distorsion->flags & EFFECT_INITIALIZED) == 0)
	{
		distorsion_init(distorsion);
		distorsion->flags |= EFFECT_INITIALIZED;
	}

	if (( distorsion->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
	{
	    distortion_process_q15(distorsion,inputData, outputData, DISTORSION_BLOCK_SIZE);
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

#endif
