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
 * echo.c
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
#include "echo.h"
#include "arm_math.h"

static float parameterValues[2];
static Parameter paramDepth = {"Feedback[%] ", 5.0f, 0.0f,  100.0f}; // 0 to 100 %
static Parameter paramDelay = {"Delay[ms]   ",  50.0f, 0.0f,  1000.0f}; // 0 to 15ms
static Parameter parameters[2];
Effect delay;

void Do_Echo(uint16_t* inputData, uint16_t* outputData, uint32_t offset)
{
	uint16_t delaySamples = ((parameterValues[1])*SAMPLE_FREQUENCY)/1000;
	float feedbackGain = (parameterValues[0]/100.0f);

	if(delay.on)
	{
		for(int i = offset; i < offset +(HALF_NUMBER_OF_AUDIO_SAMPLES); i++)
		{
			// current sample
			int16_t sample =  (int16_t)outputData[i];

			// previous sample
			int16_t prevSample = (int16_t)outputData[(i + NUMBER_OF_AUDIO_SAMPLES - delaySamples) % NUMBER_OF_AUDIO_SAMPLES];

			// apply feedback gain
			prevSample *= feedbackGain;

			// mix samples together
			outputData[i] = (uint16_t) (sample + prevSample);
		}
	}
}

void Echo_init(void)
{
	// init params
	parameters[0] = paramDepth;
	parameters[1] = paramDelay;

	parameterValues[0] = 50.0f;
	parameterValues[1] = 500.0f;

	// init effect object
	strcpy( delay.name, "Delay" );
	delay.on = 0;
	delay.currentParam = 0;
	delay.paramNum = 2;
	delay.parameters = parameters;
	delay.processBuffer = Do_Echo;
	delay.paramValues = parameterValues;
}


#endif
