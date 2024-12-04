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
 * noise.c
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
//#include "../../kernel/kernel_opt.h"

#ifdef STM32H7xx_HAL_I2S_H
#include "../audio.h"
#include "../effects.h"

#include "oscillators.h"
#include "oscillator_core.h"
#include "noise.h"

#ifdef STM32H7xx_HAL_RNG_H
extern	RNG_HandleTypeDef hrng;
#endif

ITCM_AREA_CODE void Do_Noise(int16_t *in, int16_t *out, uint8_t index)
{
uint32_t	i;
#ifdef STM32H7xx_HAL_RNG_H
uint32_t random_number;
#endif
NOISE_Gen_TypeDef	*NOISE_Gen = (NOISE_Gen_TypeDef *)Effects[index].private_data;

	if (( NOISE_Gen->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
	{
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i+=2)
		{
	#ifdef	STM32H7xx_HAL_RNG_H
			HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
			out[i] =  (random_number>>16) & 0xffff;
			out[i+1] =  random_number & 0xffff;
	#else
			out[i] =  (rand()>>16) & 0xffff;
			out[i+1] =  rand() & 0xffff;
	#endif
		}
	}
	else
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			out[i] = in[i];

}


#endif // #ifdef STM32H7xx_HAL_I2S_H
