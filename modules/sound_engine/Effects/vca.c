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
 * vca.c
 *
 *  Created on: Apr 24, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

#include "../sound_engine.h"
#include "effects.h"
#include "vca.h"

ITCM_AREA_CODE void Effect_VCA_Init(uint32_t *effect_s)
{
VCA_Effect_TypeDef *vca = (VCA_Effect_TypeDef *)effect_s;
	if ( vca->block_size == 0 )
		vca->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	vca->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_VCA(uint32_t *effect_s)
{
uint32_t	i;
VCA_Effect_TypeDef *vca = (VCA_Effect_TypeDef *)effect_s;

	if ((( vca->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( vca == NULL ))
		return;
	vca->time_start = DWT->CYCCNT;
	if (( vca->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		float gain = (float )(*vca->amplitude - *vca->offset) / FULL_SCALE_F_FACTOR;
		for ( i=0;i<vca->block_size;i++)
			vca->out_buf[i]  = (q15_t )((float )vca->in_buf[i]*gain);
	}
	else
	{
		for ( i=0;i<vca->block_size;i++)
			vca->out_buf[i]  = vca->in_buf[i];
	}
	vca->effect_time = (DWT->CYCCNT - vca->time_start) / (HSI_CLOCK / 1000000);
}

#endif // #ifdef SOUND_ENABLED
