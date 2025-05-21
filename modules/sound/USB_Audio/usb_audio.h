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
 * usb_audio.h
 *
 *  Created on: May 21, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_USB_AUDIO_USB_AUDIO_H_
#define MODULES_SOUND_USB_AUDIO_USB_AUDIO_H_

#ifdef	USB_AUDIO

typedef struct {
	uint8_t				status;
	uint16_t			out_device;		/* for dac is 32768 , for codec is 0 */
	int16_t				*out_buf;
	uint8_t				i2s_handle;
} USBAudio_TypeDef;
/* status */
#define		USBAUDIO_ENABLED		0x01
/* out_device */
#define		USBAUDIO_DAC_OUT		32768
#define		USBAUDIO_I2S_OUT		0

extern	uint8_t Usb_AUDIO_OUT_Init(USBAudio_TypeDef *usb_audio);
extern	uint8_t Usb_AUDIO_OUT_Start(USBAudio_TypeDef *usb_audio);
extern	int32_t Usb_AUDIO_OUT_Play(uint8_t* pData, uint32_t len);
#endif //#ifdef	USB_AUDIO

#endif /* MODULES_SOUND_USB_AUDIO_USB_AUDIO_H_ */
