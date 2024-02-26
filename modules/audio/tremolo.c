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

#include "effects.h"

ITCM_AREA_CODE	void Do_Tremolo(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;

	if ( (Effect[TREMOLO_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		static float phase = 0;
		float lfoFreq = Effect[TREMOLO_EFFECT_ID].parameter[1] ;
		float lfoDepth = (Effect[TREMOLO_EFFECT_ID].parameter[0]/100.0f);

		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
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
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Tremolo_init(uint32_t Rate,uint32_t Depth)
{
/*
typical values
	parameter[0] = "Depth[%]", 10.f, 0.0f, 100.f;
	parameter[1] = "Rate[Hz]", 0.5f, 1.0f,  7.0f;
*/

	Effect[TREMOLO_EFFECT_ID].parameter[0] = (float )Depth;
	Effect[TREMOLO_EFFECT_ID].parameter[1] = (float )Rate;
	Effect[TREMOLO_EFFECT_ID].num_params = 2;
	sprintf(Effect[TREMOLO_EFFECT_ID].effect_name,"Tremolo");
	sprintf(Effect[TREMOLO_EFFECT_ID].effect_param[0],"Depth");
	sprintf(Effect[TREMOLO_EFFECT_ID].effect_param[1],"Rate");
	Effect[TREMOLO_EFFECT_ID].apply_effect =  Do_Tremolo;
	Effect[TREMOLO_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

void Tremolo_enable(void)
{
	Effect[TREMOLO_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Tremolo_disable(void)
{
	Effect[TREMOLO_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}
#endif
