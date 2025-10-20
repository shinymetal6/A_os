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
	if ( *overdrive->overdrive == 0 )
		overdrive->f_overdrive = 1.0F;
	else
		overdrive->f_overdrive = (float )*overdrive->overdrive;

	return __FLOAT_2_Q15((tanhf(input) * overdrive->f_overdrive));
}

ITCM_AREA_CODE void Effect_Overdrive_Init(uint32_t *effect_s)
{
OVERDRIVE_Effect_TypeDef *overdrive = (OVERDRIVE_Effect_TypeDef *)effect_s;
	if ( overdrive->overdrive == NULL )
		return;
	overdrive->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_Overdrive(uint32_t *effect_s)
{
uint32_t	i;
OVERDRIVE_Effect_TypeDef *overdrive = (OVERDRIVE_Effect_TypeDef *)effect_s;

	if ( overdrive == NULL )
		return;

	for ( i=0;i<SOUND_BLOCK_SIZE;i++)
	{
		if (( overdrive->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			overdrive->effect_out_buf[i] = overdrive_effect(overdrive,__Q15_2_FLOAT(overdrive->effect_in_buf[i]));
		else
			overdrive->effect_out_buf[i]  = overdrive->effect_in_buf[i];
	}
}
#endif // #ifdef SOUND_ENGINE_ENABLED
