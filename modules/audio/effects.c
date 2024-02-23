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
OSCILLATORS_RAM	EffectsPipeTypeDef	EffectsPipe[MAX_EFFECTS];
OSCILLATORS_RAM	static uint32_t	effects_in_pipe = 0;

void EffectsSequencer(int16_t* inputData, int16_t* outputData)
{
uint16_t i=0;
	while(EffectsPipe[i].execute_effect != NULL)
	{
		EffectsPipe[i].execute_effect(inputData, outputData);
		i++;
		if ( i > WAH_EFFECT_ID )
			return;
	}
}

void ResetEffectsSequencer(void)
{
uint16_t i=0;
	for(i=0;i<MAX_EFFECTS;i++)
		EffectsPipe[i].execute_effect = 0;
	effects_in_pipe = 0;
}

void InsertEffect(void 	(*do_effect))
{
	EffectsPipe[effects_in_pipe].execute_effect = do_effect;
	effects_in_pipe++;
}


#endif
