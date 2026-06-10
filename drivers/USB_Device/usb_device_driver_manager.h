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
 * usb_device_driver_manager.h
 *
 *  Created on: Nov 7, 2024
 *      Author: fil
 */
#ifndef DRIVERS_USB_DRIVER_MANAGER_H_
#define DRIVERS_USB_DRIVER_MANAGER_H_

#define		USB_DRIVER_RX_SIZE		64
#define		USB_DRIVER_TX_SIZE		64

#ifdef SOUND_ENGINE_ENABLED
#include "../../modules/sound_engine/sound_engine.h"
#endif // #ifdef SOUND_ENGINE_ENABLED

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint16_t	rx_num_chars;
	uint16_t	requested_len;
	uint16_t	data_index;
	uint8_t		*data;
	uint16_t	timeout;
	uint16_t	timeout_reload_value;
	uint16_t	usb_interface_class;
	void		(*Rx_CallbackPtr)(uint8_t* buf, uint16_t len);
	uint32_t 	wakeup_id;
#ifdef SOUND_ENGINE_ENABLED
	AUDIO_Source_TypeDef 	*out_device_ptr;
#endif // #ifdef SOUND_ENGINE_ENABLED
}USB_DriverStruct_t;


#define	USB_CDC_CLASS	0
#define	USB_MIDI_CLASS	1
#define	USB_AUDIO_CLASS	2
#define	USB_UVC_CLASS	3

extern	uint32_t	usb_device_driver_register(USB_DriverStruct_t *usb_driver_private_data);
extern	uint32_t	usb_device_driver_unregister(USB_DriverStruct_t *usb_drv);

extern	uint32_t 	usb_device_driver_set_rx_buffer(USB_DriverStruct_t *usb_drv,uint8_t *rx_buf);
extern 	uint16_t 	usb_get_rx_len(USB_DriverStruct_t *usb_drv);
extern	uint32_t 	usb_send(USB_DriverStruct_t *usb_drv,uint8_t* ptr, uint16_t len);
extern	uint8_t 	MX_Aos_USB_Device_Init(uint8_t usb_classdev);
extern	void 		VIDEO_Itf_SetPtr(uint8_t *jpegdata_ptr, uint32_t jpeg_len);
extern	void 		VIDEO_Itf_StartStreaming(void);

#endif /* DRIVERS_USB_DRIVER_MANAGER_H_ */
