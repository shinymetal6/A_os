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
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../kernel/kernel_opt.h"
#ifdef USB_DEVICE_ENABLED

#include <string.h>
#include "usb_device_driver_manager.h"

SYSTEM_RAM	USB_DriverStruct_t	USB_DriverStruct;
SYSTEM_RAM	uint8_t				usb_driver_request = 0;

extern	void USB_Driver_RxTimeoutCheckCallback(void);

ITCM_AREA_CODE uint32_t	usb_device_driver_register(USB_Drv_TypeDef *usb_driver_private_data)
{
	if ( USB_DriverStruct.process == 0 )
	{
		USB_DriverStruct.process = get_current_process();
		USB_DriverStruct.status = DRIVER_STATUS_REQUESTED;
		USB_DriverStruct.usb_driver_private_data = usb_driver_private_data;
		USB_Drv_TypeDef	*usb_Drv = (USB_Drv_TypeDef	*)USB_DriverStruct.usb_driver_private_data;
		if ( usb_Drv->requested_len )
			usb_Drv->timeout_reload_value = usb_Drv->timeout;
		else
			usb_Drv->timeout_reload_value = usb_Drv->timeout;
		usb_driver_request = 1;
		set_before_check_timers_callback(USB_Driver_RxTimeoutCheckCallback);
		return 0;
	}
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t usb_device_driver_set_rx_buffer(uint8_t handle,uint8_t *rx_buf)
{
USB_Drv_TypeDef	*usb_Drv = (USB_Drv_TypeDef	*)USB_DriverStruct.usb_driver_private_data;
	usb_Drv->data = rx_buf;
	return 0;
}

ITCM_AREA_CODE uint16_t usb_get_rx_len(uint8_t handle)
{
USB_Drv_TypeDef	*usb_Drv = (USB_Drv_TypeDef	*)USB_DriverStruct.usb_driver_private_data;
	return usb_Drv->rx_num_chars;
}

#ifdef	STM32U575xx
extern	uint8_t CDC_Transmit_HS(uint8_t* Buf, uint16_t Len);
#else
extern	uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
#endif

ITCM_AREA_CODE uint32_t usb_send(uint8_t handle,uint8_t* ptr, uint16_t len)
{
#ifdef	STM32U575xx
	return (uint32_t )CDC_Transmit_HS(ptr, len);
#else
	return (uint32_t )CDC_Transmit_FS(ptr, len);
#endif
}

/* callback from timer's timeout */
ITCM_AREA_CODE void USB_Driver_RxTimeoutCheckCallback(void)
{
USB_Drv_TypeDef	*usb_Drv = (USB_Drv_TypeDef	*)USB_DriverStruct.usb_driver_private_data;
	if ( usb_Drv->timeout )
	{
		usb_Drv->timeout--;
		if ( usb_Drv->timeout == 0 )
		{
			if (  usb_Drv->data_index )
			{
				usb_Drv->rx_num_chars = usb_Drv->data_index;
				usb_Drv->data_index = 0;
				activate_process(USB_DriverStruct.process,WAKEUP_FROM_USB_DEVICE_IRQ,WAKEUP_FLAGS_HW_USB_RX_COMPLETE | WAKEUP_FLAGS_HW_USB_RX_TO);
			}
			usb_Drv->timeout = usb_Drv->timeout_reload_value;
		}
	}
}

/* callback from CDC interface */
ITCM_AREA_CODE uint32_t usb_device_driver_pktreceived_callback(uint8_t* Buf, uint32_t Len)
{
uint32_t	i;
USB_Drv_TypeDef	*usb_Drv = (USB_Drv_TypeDef	*)USB_DriverStruct.usb_driver_private_data;
	if ( usb_Drv->data == NULL )
		return 0;
	usb_Drv->timeout = usb_Drv->timeout_reload_value;
	if ( usb_Drv->requested_len )
	{
		for(i=0;i<Len;i++)
		{
			usb_Drv->data[usb_Drv->data_index] = Buf[i];
			usb_Drv->data_index++;
			if ( usb_Drv->data_index >= usb_Drv->requested_len)
			{
				usb_Drv->rx_num_chars = usb_Drv->data_index;
				usb_Drv->data_index = 0;
				activate_process(USB_DriverStruct.process,WAKEUP_FROM_USB_DEVICE_IRQ,WAKEUP_FLAGS_HW_USB_RX_COMPLETE);
			}
		}
	}
	else
	{
		for(i=0;i<Len;i++)
			usb_Drv->data[usb_Drv->data_index] = Buf[i];
		usb_Drv->rx_num_chars = Len;
		activate_process(USB_DriverStruct.process,WAKEUP_FROM_USB_DEVICE_IRQ,WAKEUP_FLAGS_HW_USB_RX_COMPLETE);
	}
	return	Len;
}

#endif // #ifdef USB_DEVICE_ENABLED



