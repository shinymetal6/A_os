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
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
//#include "../../kernel/kernel_opt.h"
#ifdef AUDIO_GENERATORS_ENABLED

#include "audio.h"
#include "effects.h"

Effects_TypeDef		Effects[MAX_EFFECTS];
uint8_t				active_effects_number = 0;

uint32_t effect_insert(void (*do_effect),uint32_t *private_data)
{
uint8_t i;
	__disable_irq();
	for(i=0;i<MAX_EFFECTS;i++)
	{
		if (Effects[i].effect == NULL )
		{
			Effects[i].private_data = private_data;
			Effects[i].effect_index = i;
			Effects[i].effect = do_effect;
			Effects[i].pipe_out = pipe[i];
			if ( i == 0 )
				Effects[i].pipe_in = oscout_buffer;
			else
				Effects[i].pipe_in = pipe[i-1];
			active_effects_number++;
			__enable_irq();
			return i;
		}
	}
	__enable_irq();
	return EFFECT_NOSPACELEFT;
}

uint32_t effects_start(int16_t *dac_buffer)
{
uint8_t i;
	for(i=0;i<MAX_EFFECTS;i++)
	{
		if (Effects[i].effect == NULL )
		{
			if ( i == 0 )
			{
				Effects[i].pipe_in = oscout_buffer;
				Effects[i].pipe_out = dac_buffer;
			}
			else
				Effects[i-1].pipe_out = dac_buffer;
			return 0;
		}
	}
	return 1;
}

void effects_apply(uint8_t full_flag)
{
uint32_t	i,j,k;

	for(k=0;k<MAX_EFFECTS;k++)
	{
		if (Effects[k].effect != NULL )
		{
			Effects[k].effect(Effects[k].pipe_in,Effects[k].pipe_out,k);
		}
		else
		{
		 	if (full_flag == 0)
			{
				for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
					Effects[k].pipe_out[i] = Effects[k].pipe_in[i];
			}
			else
			{
				for(i=HALF_NUMBER_OF_AUDIO_SAMPLES,j=0;i<NUMBER_OF_AUDIO_SAMPLES;i++,j++)
					Effects[k].pipe_out[i] = Effects[k].pipe_in[j];
			}
		 	return;
		}
	}
}


#endif // #ifdef AUDIO_GENERATORS_ENABLED

