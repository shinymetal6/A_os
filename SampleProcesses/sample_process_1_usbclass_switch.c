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
 * sample_process_1_usbclass_switch.c
 *
 *  Created on: Oct 2, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_SWITCH_CLASS

#define	USB_BUF_LEN	64
uint8_t	usb_rx_buffer[USB_BUF_LEN];
uint8_t	usb_tx_buffer[USB_BUF_LEN];
USB_Drv_TypeDef	USB_Drv =
{
		.data = usb_rx_buffer,
		.data_index = 0,
		.requested_len = USB_BUF_LEN,
		.usb_interface_class = USB_CDC_CLASS,
		.timeout = 250,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t		usb_driver_handle;

void sample_process_1_init(uint32_t process_id)
{
	usb_driver_handle = usb_device_driver_register(&USB_Drv);
}

uint8_t	switch2midi = 0, tim_downscale = 0;

void sample_process_1_usbclass_switch(uint32_t process_id)
{
uint32_t	wakeup,flags;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			if ( switch2midi == 1 )
			{
				if ( tim_downscale == 0 )
				{
					USB_Drv.usb_interface_class = USB_MIDI_CLASS;
					usb_device_driver_unregister();
				}
				tim_downscale ++;
				if ( tim_downscale > 5 )
				{
					usb_driver_handle = usb_device_driver_register(&USB_Drv);
					tim_downscale = 0;
					switch2midi = 0;
				}
			}
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			if (( usb_rx_buffer[0] == '<') && ( usb_rx_buffer[1] == 'M') && ( usb_rx_buffer[2] == 'I') && ( usb_rx_buffer[3] == 'D') && ( usb_rx_buffer[4] == 'I'))
			{
				switch2midi = 1;
			}
		}
	}
}

#endif // #ifdef SAMPLEPROCESS_1_SWITCH_CLASS
#endif // #ifdef SAMPLE_PROCESSES_ENABLED


