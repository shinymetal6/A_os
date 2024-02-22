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
 * tremolo.c
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */
/*
 * https://github.com/JoeKenyon/GuitarEffectsPedal
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effect.h"
#include "tremolo.h"
#include "arm_math.h"


#define TREMDEPTHIDX 0
#define TREMRATEIDX  1
static float parameterValues[2];
static Parameter paramDepth = {"Depth[%]", 10.f, 0.0f, 100.f};
static Parameter paramRate  = {"Rate[Hz]", 0.5f, 1.0f,  7.0f};
static Parameter parameters[2];
Effect tremolo;

/*
 *
 * @brief This function will perfrom the tremolo effect
 *        on the data passed in.
 *
 * @param inputData Pointer to the block of input data
 * @param outputData Pointer to the block of output data
 * @param offset The offset into the sample buffer
 *
 * @retval None
 *
 */
void Do_Tremolo(uint16_t* inputData, uint16_t* outputData, uint32_t offset)
{
	if(tremolo.on)
	{
		static float phase = 0;
		float lfoFreq = parameterValues[TREMRATEIDX];
		float lfoDepth = (parameterValues[TREMDEPTHIDX]/100.0f);

		for(int i = offset; i < offset+(HALF_NUMBER_OF_AUDIO_SAMPLES); i++)
		{
			// get current sample
			int16_t sample = (int16_t) outputData[i];

			// modulate sample volume with lfo
			sample *= (1.0-lfoDepth) + lfoDepth * arm_cos_f32(2*PI*phase);

			// send to output
			outputData[i] = (uint16_t) sample;

			// update phase
			phase =  fmodf(phase + lfoFreq / 44100,1);
		}
	}
}

void Tremolo_init(void)
{
	// init params
	parameters[TREMDEPTHIDX] = paramDepth;
	parameters[TREMRATEIDX] = paramRate;

	parameterValues[TREMDEPTHIDX] = 50.f;
	parameterValues[TREMRATEIDX] = 5.0f;

	// init effect object
	strcpy( tremolo.name, "Tremolo" );
	tremolo.on = 0;
	tremolo.currentParam = 0;
	tremolo.paramNum = 2;
	tremolo.parameters = parameters;
	tremolo.processBuffer = Do_Tremolo;
	tremolo.paramValues = parameterValues;
}

#endif
