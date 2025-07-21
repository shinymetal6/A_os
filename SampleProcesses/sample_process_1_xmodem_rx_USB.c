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

#define	USB_BUF_LEN	XMODEM_LINE_LEN
uint8_t	usb_rx_buffer[XMODEM_LINE_LEN];
uint8_t	usb_tx_buffer[XMODEM_LINE_LEN];

USB_Drv_TypeDef	Usb_channel =
{
		.requested_len = XMODEM_LINE_LEN,
		.data = usb_rx_buffer,
		.timeout = 10,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t	usb_handle;

XMODEM_Mod_TypeDef	XMODEM_Mod =
{
		.xmodem_rx_to = 100,
		.xmodem_rx_data_area = 0x30001000,
		.xmodem_rx_data_len = 0x2ffff,
		.xmodem_buffer = usb_rx_buffer,
		.xmodem_dev = XMODEM_DEV_USB,
};

void sample_process_1_xmodem_rx_USB(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cpoll_ena=0;
	usb_handle = usb_device_driver_register(&Usb_channel);
	XMODEM_Mod.xmodem_dev_handle = usb_handle;

	xmodem_rx_register(&XMODEM_Mod);

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			cpoll_ena++;
			if ( cpoll_ena == 10 )
				xmodem_rx_enable_poll(XMODEM_ENABLE_POLL);
			if ( cpoll_ena > 10 )
			{
				xmodem_rx(XMODEM_WAKEUP_TIMER);
				cpoll_ena = 11;
			}
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			xmodem_rx_enable_poll(0);
			if ( xmodem_rx(XMODEM_WAKEUP_DEVICE) == X_EOT )
				xmodem_rx_enable_poll(XMODEM_ENABLE_POLL);
			if ( xmodem_rx(XMODEM_WAKEUP_DEVICE) == X_DEL )
			{
				xmodem_rx_init((uint8_t *)XMODEM_Mod.xmodem_rx_data_area,XMODEM_Mod.xmodem_rx_data_len);
				xmodem_rx_enable_poll(XMODEM_ENABLE_POLL);
			}
		}
	}
}

#else
void sample_process_1_xmodem_rx_USB(uint32_t process_id)
{
	wait_event(HW_SLEEP_FOREVER);
}
#endif // #ifdef 	SAMPLEPROCESS_1_XMODEM

#endif // #ifdef SAMPLE_PROCESSES_ENABLED



