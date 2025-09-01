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
 * sample_process_1_xmodem_rx_USB.c
 *
 *  Created on: Feb 12, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_XMODEM_RX_USB

#define	xmodem_rx_data_area	0x30000000
#define	xmodem_rx_data_len		0x2ffff

#define	USB_BUF_LEN	XMODEM_LINE_LEN
uint8_t	usb_rx_buffer[XMODEM_LINE_LEN];
uint8_t	usb_tx_buffer[XMODEM_LINE_LEN];

USB_Drv_TypeDef	USB_Drv =
{
		.data = usb_rx_buffer,
		.data_index = 0,
		.requested_len = XMODEM_LINE_LEN,
		.timeout = 250,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t		usb_driver_handle;

uint8_t		xmodem_rx_usb_enable_poll;
uint8_t		tim_downscale=0;

void sample_process_1_init(uint32_t process_id)
{
	usb_driver_handle = usb_device_driver_register(&USB_Drv);
}

void sample_process_1_xmodem_rx_USB(uint32_t process_id)
{
uint32_t	wakeup,flags;

	xmodem_rx_usb_enable_poll = 1;

	xmodem_rx_init((uint8_t *)xmodem_rx_data_area,xmodem_rx_data_len);

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( xmodem_rx_usb_enable_poll	 == 1 )
			{
				tim_downscale ++;
				if ( tim_downscale > 100 )
				{
					xmodem_data_process(xmodem_rx_usb_enable_poll,XMODEM_IF_USB,usb_driver_handle,usb_rx_buffer);
					tim_downscale = 0;
				}
			}
			process_led();
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			if ( xmodem_data_process(xmodem_rx_usb_enable_poll,XMODEM_IF_USB,usb_driver_handle,usb_rx_buffer) == X_EOT)
				xmodem_rx_usb_enable_poll = 1;
			else
				xmodem_rx_usb_enable_poll = 0;
		}
	}
}

#endif // #ifdef SAMPLEPROCESS_1_XMODEM_RX_USB
#endif // #ifdef SAMPLE_PROCESSES_ENABLED



