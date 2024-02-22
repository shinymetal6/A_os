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

#include "effect.h"
#include "distortion.h"
#include "arm_math.h"

#define DISTORTIONCLIPIDX 0
#define DISTORTIONGAINIDX  1
static float parameterValues[2];
static Parameter paramClip  = {"Clipping[%]", 10.f, 0.0f, 100.f};
static Parameter paramGain  = {"Gain[%]    ", 10.0f, 100.0f, 350.0f};
static Parameter parameters[2];
Effect distortion;

// threshold_min - ( (0/100) * x)  = threshold_min
// threshold_min - ( (100/100) * x)  = threshold_max
static float clipping = 50.0f; // 50%
static float gain = 150.0f;
static const float clipping_coef = 20000.0f - 15000.0f;

void Do_Distortion(uint16_t* inputData, uint16_t* outputData, uint32_t offset)
{
	if(distortion.on)
	{
		gain = parameterValues[DISTORTIONGAINIDX]/100;
		clipping = parameterValues[DISTORTIONCLIPIDX];

		int16_t threshold = 10000.0f - ((clipping/100) * clipping_coef);

		for(int i = offset; i < offset+(HALF_NUMBER_OF_AUDIO_SAMPLES); i++)
		{
			// get current sample
			int16_t sample = (int16_t)outputData[i];

			// apply input gain
			sample *= gain;

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
}

void Distortion_init(void)
{
	// init params
	parameters[DISTORTIONCLIPIDX] = paramClip;
	parameters[DISTORTIONGAINIDX] = paramGain;

	parameterValues[DISTORTIONCLIPIDX] = clipping;
	parameterValues[DISTORTIONGAINIDX] = gain;

	// init effect object
	strcpy( distortion.name, "DISTORTION" );
	distortion.on = 0;
	distortion.currentParam = 0;
	distortion.paramNum = 2;
	distortion.parameters = parameters;
	distortion.processBuffer = Do_Distortion;
	distortion.paramValues = parameterValues;
}

#endif
