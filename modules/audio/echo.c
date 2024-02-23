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

#include "effects.h"

ITCM_AREA_CODE void Do_Echo(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;
	float feedbackGain = Effect[ECHO_EFFECT_ID].parameter[0];
	uint16_t delaySamples = (uint16_t ) Effect[ECHO_EFFECT_ID].parameter[1];
	if ( (Effect[ECHO_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
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
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Echo_init(uint32_t Feedback,uint32_t Delay)
{
/*
"Feedback[%]", 5.0f, 0.0f,  100.0f}; // 5 to 100 %
"Delay[ms]   ",  50.0f, 0.0f,  1000.0f}; // 0 to 15ms
*/

	Effect[ECHO_EFFECT_ID].parameter[0] = (float )Feedback / 100.0F;
	Effect[ECHO_EFFECT_ID].parameter[1] = (float )Delay * (float )SAMPLE_FREQUENCY / 1000.0F;
	Effect[ECHO_EFFECT_ID].num_params = 2;
	sprintf(Effect[ECHO_EFFECT_ID].effect_name,"Echo");
	sprintf(Effect[ECHO_EFFECT_ID].effect_param[0],"Feedback");
	sprintf(Effect[ECHO_EFFECT_ID].effect_param[1],"Delay[ms]");
	Effect[ECHO_EFFECT_ID].do_effect =  Do_Echo;
	Effect[ECHO_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}


void Echo_enable(void)
{
	Effect[ECHO_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Echo_disable(void)
{
	Effect[ECHO_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif
