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
 *  Created on: Nov 29, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
//#include "../../kernel/kernel_opt.h"

#include "../audio.h"
#include "../effects.h"
#include "echo.h"

ITCM_AREA_CODE void Do_Echo(int16_t *inputData, int16_t *outputData, uint8_t index)
{
uint32_t	i;
Echo_Effect_TypeDef	*echo_Effect = (Echo_Effect_TypeDef *)Effects[index].private_data;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( echo_Effect->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
		{
			// current sample
			int16_t curSample =  outputData[i];
			// previous sample
			int16_t prevSample = (int16_t)outputData[(i + NUMBER_OF_AUDIO_SAMPLES - echo_Effect->delaySamples) % NUMBER_OF_AUDIO_SAMPLES];

			// apply feedback gain
			//prevSample *= echo_Effect->feedbackGain;

			// mix samples together
			outputData[i] = (uint16_t) (curSample/2 + prevSample/2);
		}
		else
			outputData[i] = inputData[i];
	}
}

