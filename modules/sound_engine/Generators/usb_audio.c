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
 * usb_audio.c
 *
 *  Created on: Dec 9, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED
#ifdef USB_DEVICE_ENABLED
#include "../sound_engine.h"
#include "usb_audio.h"

ITCM_AREA_CODE uint8_t USBAudio_Register(AUDIO_Source_TypeDef *usbaudio_in)
{
	if ( usbaudio_in->out_buf == NULL )
		return 1;
	if ( usbaudio_in->source == AUDIO_SOURCE_LEFT)
	{
		if ( AudioSourceLeft == NULL )
		{
			AudioSourceLeft = usbaudio_in;
			usbaudio_in->next_source = NULL;
		}
		else
		{
			AUDIO_Source_TypeDef *source = AudioSourceLeft;
			while(source->next_source != NULL)
				source = (AUDIO_Source_TypeDef *)source->next_source;
			source->next_source = (uint32_t *)usbaudio_in;
			usbaudio_in->next_source = NULL;
		}
	}
	usbaudio_in->source_type = SOUND_SOURCE_IS_USB;
	usbaudio_in->block_size = I2S_EFFECT_SIZE;
	return 0;
}

#endif // #ifdef USB_DEVICE_ENABLED
#endif // #ifdef SOUND_ENGINE_ENABLED

