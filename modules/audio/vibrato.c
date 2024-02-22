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
 * vibrato.c
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
#include "vibrato.h"
#include "arm_math.h"

static float parameterValues[3];
static Parameter paramRate  = {"Rate[Hz] ",  1.0f, 4.0f,  15.0f}; // 0 to 1 hz
static Parameter paramDepth = {"Depth[%] ", 10.0f, 0.0f,  100.0f}; // 0 to 100 %
static Parameter paramDelay = {"Delay[ms]",  1.0f, 4.0f,  10.0f}; // 0 to 15ms
static Parameter parameters[3];
Effect vibrato;


void Do_Vibrato(uint16_t* inputData, uint16_t* outputData, uint32_t offset)
{
	if(vibrato.on)
	{
		static float phase = 0;
		uint16_t maxDelay  = ((parameterValues[2])*SAMPLE_FREQUENCY)/1000;
		float lfoFreq = parameterValues[0];
		float lfoDepth = (parameterValues[1]/100.0f);

		for(int i = offset; i < offset+(HALF_NUMBER_OF_AUDIO_SAMPLES); i++)
		{
			// get the modulated delay
			uint16_t delaySamples = (uint16_t)(1+(maxDelay/2)*(1-(lfoDepth * arm_cos_f32(2*PI*phase))));

			// get the delayed sample
			int16_t prevSample = (int16_t)inputData[(i + NUMBER_OF_AUDIO_SAMPLES - delaySamples) % NUMBER_OF_AUDIO_SAMPLES];

			// send to output
			outputData[i] = (uint16_t) prevSample;

			// update phase
			phase =  fmodf(phase + lfoFreq / 44100,1);
		}
	}
}

void Vibrato_init(void)
{
	// init params
	parameters[0] = paramRate;
	parameters[1] = paramDepth;
	parameters[2] = paramDelay;

	parameterValues[0] = 5.0f;
	parameterValues[1] = 50.0f;
	parameterValues[2] = 5.0f;

	// init effect object
	strcpy( vibrato.name, "VIBRATO" );
	vibrato.on = 0;
	vibrato.currentParam = 0;
	vibrato.paramNum = 3;
	vibrato.parameters = parameters;
	vibrato.processBuffer = Do_Vibrato;
	vibrato.paramValues = parameterValues;
}

#endif
