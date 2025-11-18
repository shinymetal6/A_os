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
 * usb_device_driver_manager.c
 *
 *  Created on: Nov 7, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#ifdef USB_DEVICE_ENABLED

#include <string.h>
#include "usb_device_driver_manager.h"
USB_Drv_TypeDef *usb_driver;

/* callback from timer's timeout */
ITCM_AREA_CODE void USB_Driver_RxTimeoutCheckCallback(uint32_t *param)
{
USB_Drv_TypeDef	*usb_drv = (USB_Drv_TypeDef	*)param;
	if ( usb_drv->data_index )
	{
		if ( usb_drv->timeout )
		{
			usb_drv->timeout--;
			if ( usb_drv->timeout == 0 )
			{
				usb_drv->rx_num_chars = usb_drv->data_index;
				usb_drv->data_index = 0;
				activate_process(usb_drv->process,WAKEUP_FROM_USB_DEVICE_IRQ,WAKEUP_FLAGS_HW_USB_RX_COMPLETE | WAKEUP_FLAGS_HW_USB_RX_TO);
				usb_drv->timeout = usb_drv->timeout_reload_value;
			}
		}
	}
}

__weak void	(*AudioRx_CallbackPtr)(uint8_t* buf, uint16_t len);

extern	void	usb_device_driver_pktreceived_callback(uint8_t* Buf, uint32_t Len);
extern	void	(*CDCRx_CallbackPtr)(uint8_t* buf, uint16_t len);
extern	void	(*MidiRx_CallbackPtr)(uint8_t* buf, uint16_t len);
extern	void	(*AudioRx_CallbackPtr)(uint8_t* buf, uint16_t len);

ITCM_AREA_CODE uint32_t	usb_device_driver_unregister(USB_Drv_TypeDef *usb_drv)
{
	return 0;
}

ITCM_AREA_CODE uint32_t	usb_device_driver_register(USB_Drv_TypeDef *usb_drv)
{
	usb_drv->process = get_current_process();
	usb_drv->status = DRIVER_STATUS_IN_USE;
	if ( usb_drv->requested_len )
		usb_drv->timeout_reload_value = usb_drv->timeout;
	else
		usb_drv->timeout_reload_value = usb_drv->timeout = 0;
	set_before_check_timers_callback(USB_Driver_RxTimeoutCheckCallback,(uint32_t *)usb_drv);
	if ( usb_drv->Rx_CallbackPtr != NULL)
	{
		if ( usb_drv->usb_interface_class == USB_CDC_CLASS )
			CDCRx_CallbackPtr = usb_drv->Rx_CallbackPtr;
		if ( usb_drv->usb_interface_class == USB_MIDI_CLASS )
			MidiRx_CallbackPtr = usb_drv->Rx_CallbackPtr;
		if ( usb_drv->usb_interface_class == USB_AUDIO_CLASS )
			AudioRx_CallbackPtr = usb_drv->Rx_CallbackPtr;
	}
	else
	{
		CDCRx_CallbackPtr = (void *)usb_device_driver_pktreceived_callback;
		MidiRx_CallbackPtr = (void *)usb_device_driver_pktreceived_callback;
		AudioRx_CallbackPtr = (void *)usb_device_driver_pktreceived_callback;
	}
	usb_driver = usb_drv;
	MX_Aos_USB_Device_Init(usb_drv->usb_interface_class);
	return 0;
}

ITCM_AREA_CODE uint32_t usb_device_driver_set_rx_buffer(USB_Drv_TypeDef *usb_drv,uint8_t *rx_buf)
{
	usb_drv->data = rx_buf;
	return 0;
}

ITCM_AREA_CODE uint16_t usb_get_rx_len(USB_Drv_TypeDef *usb_drv)
{
	return usb_drv->rx_num_chars;
}


#ifdef	STM32U575xx
extern	uint8_t CDC_Transmit_HS(uint8_t* Buf, uint16_t Len);
#else
extern	uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
#endif

ITCM_AREA_CODE uint32_t usb_send(USB_Drv_TypeDef *usb_drv,uint8_t* ptr, uint16_t len)
{
#ifdef	STM32U575xx
	return (uint32_t )CDC_Transmit_HS(ptr, len);
#else
	return (uint32_t )CDC_Transmit_FS(ptr, len);
#endif
}

/* callback from CDC interface */
ITCM_AREA_CODE void usb_device_driver_pktreceived_callback(uint8_t* Buf, uint32_t Len)
{
uint32_t	i;
USB_Drv_TypeDef	*usb_drv = usb_driver;
	if ( usb_drv->data == NULL )
		return;
	usb_drv->timeout = usb_drv->timeout_reload_value;
	if ( usb_drv->requested_len )
	{
		for(i=0;i<Len;i++)
		{
			usb_drv->data[usb_drv->data_index] = Buf[i];
			usb_drv->data_index++;
			if ( usb_drv->data_index >= usb_drv->requested_len)
			{
				usb_drv->rx_num_chars = usb_drv->data_index;
				usb_drv->data_index = 0;
				activate_process(usb_drv->process,WAKEUP_FROM_USB_DEVICE_IRQ,WAKEUP_FLAGS_HW_USB_RX_COMPLETE);
			}
		}
	}
	else
	{
		for(i=0;i<Len;i++)
			usb_drv->data[usb_drv->data_index] = Buf[i];
		usb_drv->rx_num_chars = Len;
		activate_process(usb_drv->process,WAKEUP_FROM_USB_DEVICE_IRQ,WAKEUP_FLAGS_HW_USB_RX_COMPLETE);
	}
}

#endif // #ifdef USB_DEVICE_ENABLED



