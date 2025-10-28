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
 * mixer.c
 *
 *  Created on: Oct 22, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

#include "../sound_engine.h"
#include "effects.h"
#include "mixer.h"

ITCM_AREA_CODE void Effect_MIXER_Init(uint32_t *effect_s)
{
MIXER_Effect_TypeDef *mixer = (MIXER_Effect_TypeDef *)effect_s;
	if ( mixer->in_buf_2ndch == NULL )
		return;
	mixer->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_MIXER(uint32_t *effect_s)
{
uint32_t	i;
MIXER_Effect_TypeDef *mixer = (MIXER_Effect_TypeDef *)effect_s;

float pan_left  = 1.0F - ((float )(*mixer->pan) / FULL_SCALE_F_FACTOR);
float pan_right = (float )(*mixer->pan) / FULL_SCALE_F_FACTOR;
	if ((( mixer->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( mixer == NULL ))
		return;
	for ( i=0;i<mixer->block_size;i++)
	{
		if (( mixer->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			mixer->out_buf[i]  = (q15_t )(((float )mixer->in_buf[i]*pan_left ) + ((float )mixer->in_buf_2ndch[i]*pan_right ) );
		else
			mixer->out_buf[i]  = mixer->in_buf[i];
	}
}

#endif // #ifdef SOUND_ENABLED
