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
 * sample_process_1_usbecho.c
 *
 *  Created on: Sep 15, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_USB_ECHO

#define	USB_BUF_LEN	64
uint8_t	usb_rx_buffer[USB_BUF_LEN];
uint8_t	usb_tx_buffer[USB_BUF_LEN];

USB_Drv_TypeDef	USB_Drv =
{
		.data = usb_rx_buffer,
		.data_index = 0,
		.requested_len = USB_BUF_LEN,
		.timeout = 100,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t		usb_driver_handle;

void sample_process_1_init(uint32_t process_id)
{
	usb_driver_handle = usb_device_driver_register(&USB_Drv);
}

void sample_process_1_usbecho(uint32_t process_id)
{
uint32_t	wakeup,flags;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			usb_send(usb_driver_handle,usb_rx_buffer, usb_get_rx_len(usb_driver_handle));
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_USB_ECHO
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

