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
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"

#ifdef SOUND_ENABLED
#ifdef	USB_AUDIO

#include "usb_audio.h"
#include "audio_fifo.h"

USBAudio_TypeDef	*usbaudio_user_struct;
uint32_t			audio_out_play=0;

ITCM_AREA_CODE uint32_t Usb_AUDIO_OUT_Play(uint8_t* pData, uint32_t len)
{
uint32_t	i;

	for(i=0;i<len;i+=2)
		usbaudio_user_struct->out_buf[i] = (pData[i+1] << 8) | pData[i+0];

	return 0;
}

ITCM_AREA_CODE static uint32_t Usb_AUDIO_OUT_Periodic(uint8_t* pData, uint32_t len, uint8_t cmd)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t Usb_AUDIO_OUT_Volume(uint8_t vol)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t Usb_AUDIO_OUT_Mute(uint8_t cmd)
{
	return 0;
}

ITCM_AREA_CODE uint32_t Usb_AUDIO_OUT_Start(USBAudio_TypeDef *usb_audio)
{
	usb_audio->status = USBAUDIO_ENABLED;
    return 0;
}

ITCM_AREA_CODE uint8_t Usb_AUDIO_OUT_Init(USBAudio_TypeDef *usb_audio,uint8_t *dac_Drv_status)
{
	usbaudio_user_struct = usb_audio;
	usb_audio->dac_drvstatus = dac_Drv_status;
	usb_audio->play = Usb_AUDIO_OUT_Play;
	usb_audio->periodic = Usb_AUDIO_OUT_Periodic;
	usb_audio->volume = Usb_AUDIO_OUT_Volume;
	usb_audio->mute = Usb_AUDIO_OUT_Mute;
    return 0;
}

ITCM_AREA_CODE int16_t *Usb_AUDIO_GetUSBbuf_ptr(void)
{
	return usbaudio_user_struct->usbaudio_buf;
}

#endif //#ifdef	USB_AUDIO
#endif // #ifdef SOUND_ENABLED
