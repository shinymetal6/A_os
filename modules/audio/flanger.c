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
 * flanger.c
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

ITCM_AREA_CODE void Do_Flanger(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;

	if ( (BlockEffect[FLANGER_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		static float phase = 0;
		uint16_t maxDelay  = ((BlockEffect[FLANGER_EFFECT_ID].parameter[2])*SAMPLE_FREQUENCY)/1000;
		float lfoFreq = BlockEffect[FLANGER_EFFECT_ID].parameter[0];
		float lfoDepth = (BlockEffect[FLANGER_EFFECT_ID].parameter[1]/100.0f);

		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			// get current sample
			int16_t sample = (int16_t)outputData[i];

			// get the modulated delay
			uint16_t delaySamples = (uint16_t)(1+(maxDelay/2)*(1-(lfoDepth * arm_cos_f32(2*PI*phase))));

			// get the delayed sample
			int16_t prevSample = (int16_t)inputData[(i + NUMBER_OF_AUDIO_SAMPLES - delaySamples) % NUMBER_OF_AUDIO_SAMPLES];

			// send to output
			outputData[i] = (uint16_t) ((sample+prevSample)/2);

			// update phase
			phase =  fmodf(phase + lfoFreq / (float)SAMPLE_FREQUENCY,1);
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Flanger_init(uint32_t Rate,uint32_t Depth, uint32_t Delay)
{
/*
{"Rate[Hz] ",  0.1f, 0.1f,  1.0f}; // 0 to 1 hz
{"Depth[%] ", 10.0f, 0.0f,  100.0f}; // 0 to 100 %
{"Delay[ms]",  1.0f, 0.0f,  15.0f}; // 0 to 15ms

typical values
	parameter[0] = 0.5f;
	parameter[1] = 50.0f;
	parameter[2] = 2.0f;
*/

	BlockEffect[FLANGER_EFFECT_ID].parameter[0] = (float )Rate;
	BlockEffect[FLANGER_EFFECT_ID].parameter[1] = (float )Depth;
	BlockEffect[FLANGER_EFFECT_ID].parameter[2] = (float )Delay;
	BlockEffect[FLANGER_EFFECT_ID].num_params = 3;
	sprintf(BlockEffect[FLANGER_EFFECT_ID].effect_name,"Flanger");
	sprintf(BlockEffect[FLANGER_EFFECT_ID].effect_param[0],"Rate");
	sprintf(BlockEffect[FLANGER_EFFECT_ID].effect_param[1],"Depth");
	sprintf(BlockEffect[FLANGER_EFFECT_ID].effect_param[1],"Delay[ms]");
	BlockEffect[FLANGER_EFFECT_ID].apply_effect =  Do_Flanger;
	BlockEffect[FLANGER_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

void Flanger_enable(void)
{
	BlockEffect[FLANGER_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Flanger_disable(void)
{
	BlockEffect[FLANGER_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif
