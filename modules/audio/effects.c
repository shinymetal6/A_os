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

OSCILLATORS_RAM	BlockEffectsTypeDef					BlockEffect[MAX_BLOCK_EFFECTS];
OSCILLATORS_RAM	SingleSampleEffectsTypeDef			SingleSampleEffect[MAX_SINGLESAMPLE_EFFECTS];

OSCILLATORS_RAM	static BlockEffectsTypeDef			*first_block_effect_ptr;
OSCILLATORS_RAM	static SingleSampleEffectsTypeDef	*first_single_sample_effect_ptr;
OSCILLATORS_RAM int16_t								pipe_index,single_sample_index, single_sample_counter;

extern	int16_t	pipe[MAX_BLOCK_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];

void BlockEffectsSequencer(int16_t *sample_in , int16_t *sample_out)
{
uint16_t 	i,k;

BlockEffectsTypeDef	*effect = first_block_effect_ptr;

	pipe_index = 0;
	for(i=0;i<MAX_BLOCK_EFFECTS;i++)
	{
		if ( effect->apply_effect != NULL )
		{
			if ((effect->effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
			{
				effect->apply_effect(pipe[pipe_index], pipe[pipe_index+1]);
			}
			else
			{
				for(k=0;k<HALF_NUMBER_OF_AUDIO_SAMPLES;k++)
					pipe[pipe_index+1][k] = pipe[pipe_index][k];
			}
			pipe_index++;
		}
		effect = (BlockEffectsTypeDef *)&effect->nxt_effect;
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


void ResetBlockEffectsSequencer(void)
{
}

void InsertBlockEffect(void 	(*do_effect),uint8_t position)
{

}

void InsertSingleSampleEffect(void 	(*do_effect),uint8_t position)
{

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
		effect ++;
	}
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

void InitEffectsSequencer(void)
{
	InitBlockEffectsSequencer();
	InitSingleSampleEffectsSequencer();
	pipe_index = single_sample_index = single_sample_counter = 0;
}

#endif
