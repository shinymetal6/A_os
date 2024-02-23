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
 * distortion.c
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effects.h"

OSCILLATORS_RAM	DistortionTypeDef	DistortionData;

// threshold_min - ( (0/100) * x)  = threshold_min
// threshold_min - ( (100/100) * x)  = threshold_max

ITCM_AREA_CODE void Do_Distortion(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;

	if ( (Effect[DISTORSION_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		DistortionData.gain = Effect[DISTORSION_EFFECT_ID].parameter[1]/100;
		DistortionData.clipping = Effect[DISTORSION_EFFECT_ID].parameter[0];

		int16_t threshold = 10000.0f - ((DistortionData.clipping/100) * DistortionData.clipping_coef);

		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			// get current sample
			int16_t sample = (int16_t)outputData[i];

			// apply input gain
			sample *= DistortionData.gain;

			// check if above threshold
			if(fabsf(sample) >= threshold)
			{
				if(sample > 0) sample = threshold;
				else sample = -threshold;
			}

			// send to output
			outputData[i] = (uint16_t)sample;
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Distortion_init(uint32_t Clipping,uint32_t Gain)
{
/*
typical values
	parameter[0] = clipping = 50.0f; // 50%
	parameter[1] = gain = 150.0f;
*/

	Effect[DISTORSION_EFFECT_ID].parameter[0] = (float )Clipping;
	Effect[DISTORSION_EFFECT_ID].parameter[1] = (float )Gain;
	Effect[DISTORSION_EFFECT_ID].num_params = 2;
	sprintf(Effect[DISTORSION_EFFECT_ID].effect_name,"Distorsion");
	sprintf(Effect[DISTORSION_EFFECT_ID].effect_param[0],"Clipping");
	sprintf(Effect[DISTORSION_EFFECT_ID].effect_param[1],"Gain");
	Effect[DISTORSION_EFFECT_ID].do_effect =  Do_Distortion;
	Effect[DISTORSION_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;

	DistortionData.clipping = 50.0f; // 50%
	DistortionData.gain = 150.0f;
	DistortionData.clipping_coef = 20000.0f - 15000.0f;
}

void Distortion_enable(void)
{
	Effect[DISTORSION_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Distortion_disable(void)
{
	Effect[DISTORSION_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}


#endif
