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

#include "effects.h"

ITCM_AREA_CODE void Do_Vibrato(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;

	if ( (Effect[VIBRATO_EFFECT_ID].effect_enabled & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		static float phase = 0;
		uint16_t maxDelay  = ((Effect[VIBRATO_EFFECT_ID].parameter[2])*SAMPLE_FREQUENCY)/1000;
		float lfoFreq = Effect[VIBRATO_EFFECT_ID].parameter[0];
		float lfoDepth = (Effect[VIBRATO_EFFECT_ID].parameter[1]/100.0f);

		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
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
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Vibrato_init(uint32_t Rate,uint32_t Depth, uint32_t Delay)
{
/*
typical values
	parameter[0] = "Depth[%]", 10.f, 0.0f, 100.f;
	parameter[1] = "Rate[Hz]", 0.5f, 1.0f,  7.0f;
*/

	Effect[VIBRATO_EFFECT_ID].parameter[0] = (float )Rate;
	Effect[VIBRATO_EFFECT_ID].parameter[1] = (float )Depth;
	Effect[VIBRATO_EFFECT_ID].parameter[2] = (float )Delay;
	Effect[VIBRATO_EFFECT_ID].num_params = 3;
	sprintf(Effect[VIBRATO_EFFECT_ID].effect_name,"Vibrato");
	sprintf(Effect[VIBRATO_EFFECT_ID].effect_param[0],"Rate");
	sprintf(Effect[VIBRATO_EFFECT_ID].effect_param[1],"Depth");
	sprintf(Effect[VIBRATO_EFFECT_ID].effect_param[1],"Delay");
	Effect[VIBRATO_EFFECT_ID].do_effect =  Do_Vibrato;
	Effect[VIBRATO_EFFECT_ID].effect_enabled = 0;
}

void Vibrato_enable(void)
{
	Effect[VIBRATO_EFFECT_ID].effect_enabled |= EFFECT_ENABLED;
}

void Vibrato_disable(void)
{
	Effect[VIBRATO_EFFECT_ID].effect_enabled &= ~EFFECT_ENABLED;
}

#endif
