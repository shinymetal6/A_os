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
 * sample_process_1_midi.c
 *
 *  Created on: Oct 2, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_MIDI

extern	void	SysEx(void);
extern	void	Note(uint8_t midi_channel , uint8_t midi_note , uint8_t midi_velocity);
extern	void	ControlChange(uint8_t cc_index,uint8_t cc_value);
extern	void	ProgramChange(uint8_t pc_index,uint8_t pc_value);

#define	USB_BUF_LEN	64

uint8_t	rx_sysex_buffer[SYSEX_MAX_LEN];
A_midi_t	MIDI =
{
		.SysEx = SysEx,
		.Note = Note,
		.ControlChange = ControlChange,
		.ProgramChange = ProgramChange,
		.midi_received_sysex_buffer = rx_sysex_buffer,
};

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
uint32_t		midi_cmd = 0;

void	SysEx(void)
{

}
void	Note(uint8_t midi_channel , uint8_t midi_note , uint8_t midi_velocity)
{
	midi_cmd++;
}

void	ControlChange(uint8_t cc_index,uint8_t cc_value)
{

}

void	ProgramChange(uint8_t pc_index,uint8_t pc_value)
{

}

void sample_process_1_init(uint32_t process_id)
{
	usb_driver_handle = usb_device_driver_register(&USB_Drv);
}

uint32_t	midi_initialized;

void sample_process_1_midi(uint32_t process_id)
{
uint32_t	wakeup,flags;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	midi_initialized = MidiInit(&MIDI);

	while(1)
	{
		wait_event(EVENT_TIMER|EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
			if ( midi_initialized == 0 )
				MidiParser(usb_rx_buffer, usb_get_rx_len(usb_driver_handle));
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_MIDI
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
