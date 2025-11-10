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
 * i2s_in.c
 *
 *  Created on: Oct 22, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_I2S_ENABLED
#include "../sound_engine.h"
#include "i2s_in.h"

ITCM_AREA_CODE uint8_t I2SIn_Start(AUDIO_Source_TypeDef *i2s_in)
{
	i2s_in->status = SOURCE_ENABLED;
    return 0;
}

ITCM_AREA_CODE uint8_t I2SIn_Stop(AUDIO_Source_TypeDef *i2s_in)
{
	i2s_in->status &= ~SOURCE_DISABLED;
    return 0;
}

ITCM_AREA_CODE uint8_t I2SIn_Register(AUDIO_Source_TypeDef *i2s_in)
{
	if ( i2s_in->out_buf == NULL )
		return 1;
	if ( i2s_in->channel_in == AUDIO_SOURCE_LEFT)
	{
		if ( AudioSourceLeft == NULL )
		{
			AudioSourceLeft = i2s_in;
			i2s_in->next_source = NULL;
		}
		else
		{
			AUDIO_Source_TypeDef *source = AudioSourceLeft;
			while(source->next_source != NULL)
				source = (AUDIO_Source_TypeDef *)source->next_source;
			source->next_source = (uint32_t *)i2s_in;
			i2s_in->next_source = NULL;
		}
	}
	i2s_in->source_type = SOUND_SOURCE_IS_I2S_IN;
	i2s_in->block_size = I2S_EFFECT_SIZE;
	return 0;
}

#endif // #ifdef SOUND_ENGINE_I2S_ENABLED



