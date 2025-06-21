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
	uint8_t				*dac_drvstatus;
	int16_t				*out_buf;
	int16_t				*usbaudio_buf;
	uint16_t 			len;
	uint16_t 			buf_index;
	uint8_t				i2s_handle;
	uint32_t			(*play)(uint8_t *buf , uint32_t len);
	uint32_t			(*periodic)( uint8_t* pData, uint32_t len, uint8_t cmd);
	uint32_t			(*volume)(uint8_t vol);
	uint32_t			(*mute)(uint8_t cmd);
	uint32_t			(*init)(uint32_t *usb_audio);
} USBAudio_TypeDef;
/* status */
#define		USBAUDIO_ENABLED		0x01
/* out_device */
#define		USBAUDIO_DAC_OUT		32768
#define		USBAUDIO_I2S_OUT		0

extern	void AUDIO_TransferComplete_CallBack_FS(void);
extern	void AUDIO_HalfTransfer_CallBack_FS(void);

extern	uint8_t Usb_AUDIO_OUT_Init(USBAudio_TypeDef *usb_audio,uint8_t *dac_Drv_status);
extern	uint32_t Usb_AUDIO_OUT_Start(USBAudio_TypeDef *usb_audio);
extern	int16_t *Usb_AUDIO_GetUSBbuf_ptr(void);

#endif //#ifdef	USB_AUDIO

#endif /* MODULES_SOUND_USB_AUDIO_USB_AUDIO_H_ */
