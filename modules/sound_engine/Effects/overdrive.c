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
 * overdrive.c
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */


#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "overdrive.h"

ITCM_AREA_CODE static q15_t overdrive_effect(OVERDRIVE_Effect_TypeDef* overdrive, float input)
{
	overdrive->f_overdrive = (float )*overdrive->overdrive / 16384.0F;
	return __FLOAT_2_Q15((fast_tanh(input) * overdrive->f_overdrive));
}

ITCM_AREA_CODE static q15_t overdrive_asymmetric(OVERDRIVE_Effect_TypeDef* overdrive, float input)
{
	overdrive->f_overdrive = (float )*overdrive->overdrive / 32768.0F;
	input *= overdrive->f_overdrive;
	if (input > 0.0f)
		return __FLOAT_2_Q15(1.0f - expf(-input));           // Soft knee for positive
	else
		return __FLOAT_2_Q15(-1.0f + expf(input)) * 0.7f;    // Softer negative (asymmetry)
}

ITCM_AREA_CODE void Effect_Overdrive_Init(uint32_t *effect_s)
{
OVERDRIVE_Effect_TypeDef *overdrive = (OVERDRIVE_Effect_TypeDef *)effect_s;
	if ( overdrive->overdrive == NULL )
		return;
	if ( overdrive->block_size == 0 )
		overdrive->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	overdrive->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_Overdrive(uint32_t *effect_s)
{
uint32_t	i;
OVERDRIVE_Effect_TypeDef *overdrive = (OVERDRIVE_Effect_TypeDef *)effect_s;
q15_t 	(*ovdrive_ptr)(OVERDRIVE_Effect_TypeDef* overdrive, float input);

	if ( overdrive == NULL )
		return;
	overdrive->time_start = DWT->CYCCNT;
	if (( overdrive->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		if (( overdrive->flags & FLAGS_OVERDIVE_ASYMMETRIC) == FLAGS_OVERDIVE_ASYMMETRIC)
			ovdrive_ptr = overdrive_asymmetric;
		else
			ovdrive_ptr = overdrive_effect;
		for ( i=0;i<overdrive->block_size;i++)
			overdrive->out_buf[i] = ovdrive_ptr(overdrive,__Q15_2_FLOAT(overdrive->in_buf[i]));
	}
	else
	{
		for ( i=0;i<overdrive->block_size;i++)
			overdrive->out_buf[i]  = overdrive->in_buf[i];
	}
	overdrive->effect_time = (DWT->CYCCNT - overdrive->time_start) / (HSI_CLOCK / 1000000);
}
#endif // #ifdef SOUND_ENGINE_ENABLED
