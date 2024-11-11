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
#include <string.h>
#include "usb_device_driver_manager.h"

extern		USB_DriverStruct_t	USB_DriverStruct;
SYSTEM_RAM	uint8_t			usb_driver_request = 0;

ITCM_AREA_CODE uint32_t	usb_device_driver_register(USB_DriverStruct_t *driver)
{
	USB_DriverStruct.process = get_current_process();
	USB_DriverStruct.status = DRIVER_STATUS_REQUESTED;
	usb_driver_request++;
	return 0;
}

ITCM_AREA_CODE uint32_t usb_device_driver_scan(void)
{
	if (usb_driver_request )
	{
		if (( USB_DriverStruct.status & DRIVER_STATUS_REQUESTED) ==  DRIVER_STATUS_REQUESTED)
		{
			USB_DriverStruct.status = DRIVER_STATUS_IN_USE;
			usb_driver_request --;
		}
	}
	return DRIVER_STATUS_INITIALIZED;
}

ITCM_AREA_CODE uint32_t usb_device_driver_set_rx_buffer(uint8_t handle,uint8_t *rx_buf)
{
	USB_DriverStruct.rx_buf = rx_buf;
	return 0;
}

ITCM_AREA_CODE uint16_t usb_device_driver_get_rx_len(uint8_t handle)
{
	return USB_DriverStruct.rx_buf_len;
}

#ifdef	STM32U575xx
extern	uint8_t CDC_Transmit_HS(uint8_t* Buf, uint16_t Len);
#else
extern	uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
#endif

ITCM_AREA_CODE uint32_t usb_device_driver_send(uint8_t handle,uint8_t* ptr, uint16_t len)
{
#ifdef	STM32U575xx
	return (uint32_t )CDC_Transmit_HS(ptr, len);
#else
	return (uint32_t )CDC_Transmit_FS(ptr, len);
#endif
}


/* callback from CDC interface */
uint32_t usb_device_driver_pktreceived_callback(uint8_t* Buf, uint32_t Len)
{
uint32_t	i;
	if ( USB_DriverStruct.rx_buf == NULL )
		return 0;
	USB_DriverStruct.rx_buf_len = Len;
	for(i=0;i<Len;i++)
		USB_DriverStruct.rx_buf[i] = Buf[i];
	//USB_DriverStruct.rx_buf[USB_DriverStruct.rx_buf_len] = 0;
	activate_process(USB_DriverStruct.process,WAKEUP_FROM_USB_DEVICE_IRQ,WAKEUP_FLAGS_HW_USB_RX_COMPLETE);
	return	Len;
}




