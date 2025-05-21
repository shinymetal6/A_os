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
 *  Created on: May 21, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"

#ifdef SOUND_ENABLED
#ifdef	USB_AUDIO

#include "usb_audio.h"

ITCM_AREA_CODE int32_t Usb_AUDIO_OUT_Play(uint8_t* pData, uint32_t len)
{
	return 0;
}

ITCM_AREA_CODE uint8_t Usb_AUDIO_OUT_Init(USBAudio_TypeDef *usb_audio)
{
    return 0;
}

ITCM_AREA_CODE uint8_t Usb_AUDIO_OUT_Start(USBAudio_TypeDef *usb_audio)
{
	usb_audio->status = USBAUDIO_ENABLED;
    return 0;
}
#endif //#ifdef	USB_AUDIO
#endif // #ifdef SOUND_ENABLED
