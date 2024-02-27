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
 * effects.c
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

OSCILLATORS_RAM	EffectsTypeDef		Effect[MAX_EFFECTS];
OSCILLATORS_RAM	static EffectsTypeDef		*first_effect_ptr;

extern	uint16_t	pipe[MAX_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];

void EffectsSequencer(void)
{
uint16_t 	i;

EffectsTypeDef	*effect = first_effect_ptr;

	for(i=0;i<MAX_EFFECTS;i++)
	{
		if ((effect->effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
		{
			effect->apply_effect((int16_t *)&pipe[i][0], (int16_t *)&pipe[i+1][0]);
		}
		effect = (EffectsTypeDef *)&effect->nxt_effect;
	}
}

void ResetEffectsSequencer(void)
{
}

void InsertEffect(void 	(*do_effect),uint8_t position)
{

}

void InitEffectsSequencer(void)
{
uint16_t 	i;
EffectsTypeDef	*effect = first_effect_ptr;

	first_effect_ptr = &Effect[0];
	effect = first_effect_ptr;
	for(i=0;i<MAX_EFFECTS;i++)
	{
		effect->nxt_effect = (uint8_t *)&Effect[i+1];
		bzero(effect->effect_name, sizeof(effect->effect_name));
		bzero(effect->effect_param, sizeof(effect->effect_param));
		bzero(effect->parameter, sizeof(effect->parameter));
		effect->num_params = effect->effect_status = 0;
		if ( i > 0 )
			effect->pre_effect = (uint8_t *)&Effect[i-1];
		else
			effect->pre_effect = NULL;
	}
}

#endif
