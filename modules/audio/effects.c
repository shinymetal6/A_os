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

AUDIO_FAST_RAM	BlockEffectsTypeDef					BlockEffect[MAX_BLOCK_EFFECTS];
AUDIO_FAST_RAM	SingleSampleEffectsTypeDef			SingleSampleEffect[MAX_SINGLESAMPLE_EFFECTS];

AUDIO_FAST_RAM	static BlockEffectsTypeDef			*first_block_effect_ptr;
AUDIO_FAST_RAM int16_t								pipe_index,single_sample_index, single_sample_counter;

AUDIO_FAST_RAM	EffectsOrderTypeDef					EffectsOrder;

extern	int16_t	pipe[MAX_BLOCK_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];

uint16_t BlockEffectsSequencer(void)
{
uint16_t 	i,k,block_effect_index;

	pipe_index = 0;
	for(i=0;i<MAX_BLOCK_EFFECTS;i++)
	{
		block_effect_index = EffectsOrder.effect_order[i];
		if ( block_effect_index == EFFECT_ORDER_NOT_USED )
			return pipe_index;
		if ( BlockEffect[block_effect_index].apply_effect != NULL )
		{
			if ((BlockEffect[block_effect_index].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
			{
				BlockEffect[block_effect_index].apply_effect(pipe[pipe_index], pipe[pipe_index+1]);
			}
			else
			{
				for(k=0;k<HALF_NUMBER_OF_AUDIO_SAMPLES;k++)
					pipe[pipe_index+1][k] = pipe[pipe_index][k];
			}
			pipe_index++;
		}
	}
	return pipe_index-1;
}

uint8_t FindBlockEffect(void 	(*do_effect))
{
uint8_t i;
	for(i=0;i<MAX_BLOCK_EFFECTS;i++)
	{
		if ( BlockEffect[i].apply_effect == do_effect)
			return i;
	}
	return EFFECT_ORDER_NOT_USED;
}

void InsertBlockEffect(void (*do_effect),uint8_t position,uint8_t enabled)
{
	EffectsOrder.effect_order[position] = FindBlockEffect(do_effect);
	BlockEffect[position].current_order = position;
	BlockEffect[position].effect_status = enabled;
}

void RemoveBlockEffect(void (*do_effect),uint8_t position)
{
uint8_t i, iposition = EffectsOrder.effect_order[position];
	if ( BlockEffect[iposition].apply_effect == do_effect)
	{
		if ( EffectsOrder.effect_order[position+1] != EFFECT_ORDER_NOT_USED)
		{
			for(i=position+1;i<MAX_BLOCK_EFFECTS;i++)
				EffectsOrder.effect_order[i-1] = EffectsOrder.effect_order[i];
		}
		else
			EffectsOrder.effect_order[position] = EFFECT_ORDER_NOT_USED;
	}
}


void InitBlockEffectsSequencer(void)
{
uint16_t 	i;
BlockEffectsTypeDef	*effect = first_block_effect_ptr;

	first_block_effect_ptr = &BlockEffect[0];
	effect = first_block_effect_ptr;
	effect->pre_effect = NULL;
	for(i=1;i<MAX_BLOCK_EFFECTS;i++)
	{
		effect->nxt_effect = (uint8_t *)&BlockEffect[i+1];
		bzero(effect->effect_name, sizeof(effect->effect_name));
		bzero(effect->effect_param, sizeof(effect->effect_param));
		bzero(effect->parameter, sizeof(effect->parameter));
		effect->num_params = effect->effect_status = 0;
		effect->pre_effect = (uint8_t *)&BlockEffect[i];
		effect->current_order = 0;
		effect ++;
		EffectsOrder.effect_order[i] = EFFECT_ORDER_NOT_USED;
	}
}

void InitEffectsSequencer(void)
{
	InitBlockEffectsSequencer();
	pipe_index = single_sample_index = single_sample_counter = 0;
}

#ifdef SSSS

AUDIO_FAST_RAM	static SingleSampleEffectsTypeDef	*first_single_sample_effect_ptr;

void ResetBlockEffectsSequencer(void)
{
}

void InitSingleSampleEffectsSequencer(void)
{
uint16_t 	i;
SingleSampleEffectsTypeDef	*effect = first_single_sample_effect_ptr;

	first_single_sample_effect_ptr = &SingleSampleEffect[0];
	effect = first_single_sample_effect_ptr;
	effect->pre_effect = NULL;
	for(i=1;i<MAX_BLOCK_EFFECTS;i++)
	{
		effect->nxt_effect = (uint8_t *)&SingleSampleEffect[i+1];
		bzero(effect->effect_name, sizeof(effect->effect_name));
		bzero(effect->effect_param, sizeof(effect->effect_param));
		bzero(effect->parameter, sizeof(effect->parameter));
		effect->num_params = effect->effect_status = 0;
		effect->pre_effect = (uint8_t *)&BlockEffect[i];
		effect ++;
	}
}


void SingleSampleEffectsSequencer(void)
{
uint16_t 	i;
SingleSampleEffectsTypeDef	*effect = first_single_sample_effect_ptr;

	for(i=0;i<MAX_SINGLESAMPLE_EFFECTS;i++)
	{
		if ((effect->effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
		{
			pipe[pipe_index+1][single_sample_counter] = effect->apply_effect(pipe[pipe_index][single_sample_counter]);
		}
		else
		{
			pipe[pipe_index+1][single_sample_counter] = pipe[pipe_index][single_sample_counter];
		}
		single_sample_counter++;
		single_sample_counter &= (HALF_NUMBER_OF_AUDIO_SAMPLES-1);
		if ( single_sample_counter == 0)
		{
			effect = (SingleSampleEffectsTypeDef *)&effect->nxt_effect;
			single_sample_index++;
			pipe_index++;
			if ( single_sample_index >= MAX_SINGLESAMPLE_EFFECTS)
				single_sample_index = 0;
		}
	}
}

void InsertSingleSampleEffect(void 	(*do_effect),uint8_t position)
{

}
#endif // #ifdef SSSS


#endif // #ifdef SYNTH_ENGINE_ENABLE

