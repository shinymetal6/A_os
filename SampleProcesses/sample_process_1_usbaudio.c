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
 * sample_process_1_usbaudio.c
 *
 *  Created on: May 19, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_USBAUDIO

#define	USB_AUDIO_LEN	512
uint8_t	usb_rx_buffer[USB_AUDIO_LEN];
uint8_t	usb_tx_buffer[USB_AUDIO_LEN];

USB_Drv_TypeDef	Usb_channel =
{
		.requested_len = USB_AUDIO_LEN,
		.data = usb_rx_buffer,
		.timeout = 10,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t	usb_handle;

void sample_process_1_usbaudio(uint32_t process_id)
{
uint32_t	wakeup,flags;

	usb_handle = usb_device_driver_register(&Usb_channel);

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
		}
	}
}
#endif // #ifdef 	SAMPLEPROCESS_1_XMODEM

#endif // #ifdef SAMPLE_PROCESSES_ENABLED



