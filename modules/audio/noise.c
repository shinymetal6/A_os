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
 *  Created on: Jan 8, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effects.h"

#ifdef	HWRANDOM_GEN
extern	RNG_HandleTypeDef hrng;
#endif

ITCM_AREA_CODE void Do_Noise(int16_t* inputData, int16_t* outputData)
{
uint32_t	i;
uint32_t random_number;
	if ( (Effect[FLANGER_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i+=2)
		{
	#ifdef	HWRANDOM_GEN
			HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
			outputData[i] =  (random_number>>16) & 0xffff;
			outputData[i+1] =  random_number & 0xffff;
	#else
			noise_buffer_gen[i] =  (rand()>>16) & 0xffff;
			noise_buffer_gen[i+1] =  rand() & 0xffff;
	#endif
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Noise_init(uint32_t Rate,uint32_t Depth, uint32_t Delay)
{
	Effect[NOISE_EFFECT_ID].num_params = 0;
	sprintf(Effect[NOISE_EFFECT_ID].effect_name,"Noise");
	Effect[NOISE_EFFECT_ID].do_effect =  Do_Noise;
	Effect[NOISE_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

void Noise_enable(void)
{
	Effect[NOISE_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Noise_disable(void)
{
	Effect[NOISE_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}


#endif // #ifdef SYNTH_ENGINE_ENABLE
