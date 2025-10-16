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
 * sample_process_1_audiomidi.c
 *
 *  Created on: Oct 15, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIO_MIDI

extern	DAC_HandleTypeDef hdac1;
extern	TIM_HandleTypeDef htim6;

#define	USB_BUF_LEN	64

#define	DUAL_CHANNEL	1

__attribute__ ((aligned (32))) int16_t	dac_buffer_left[DAC_AUDIO_BUF_SIZE*2];
__attribute__ ((aligned (32))) int16_t	synth_workbuffer_left[NUMBER_OF_AUDIO_SAMPLES];
#ifdef DUAL_CHANNEL
__attribute__ ((aligned (32))) int16_t	dac_buffer_right[DAC_AUDIO_BUF_SIZE*2];
__attribute__ ((aligned (32))) int16_t	synth_workbuffer_right[NUMBER_OF_AUDIO_SAMPLES];
#endif // #ifdef DUAL_CHANNEL

#define	LEFT_CHANNEL	0
#define	RIGHT_CHANNEL	1

#define	SAMPLE_FREQUENCY	96000

uint8_t	usb_rx_buffer[USB_BUF_LEN];
uint8_t	usb_tx_buffer[USB_BUF_LEN];

__attribute__ ((aligned (32)))	USB_Drv_TypeDef	USB_Drv =
{
		.data = usb_rx_buffer,
		.data_index = 0,
		.usb_interface_class = USB_MIDI_CLASS,
		.requested_len = USB_BUF_LEN,
		.timeout = 100,
		.wakeup_id = WAKEUP_FROM_USB_DEVICE_IRQ,
};
uint32_t		usb_driver_handle;

__attribute__ ((aligned (32)))	MidiSynth_TypeDef Audio_Synth_left =
{
	.status = SYNTH_DISABLED,
	.out_buf = synth_workbuffer_left,
	.out_device = SYNTH_DAC_OUT,
	.codec_buf = dac_buffer_left,
	.sample_rate = SAMPLE_FREQUENCY,
	.wavetable_size = SYNTH_WAVETABLE_1024,
};
uint32_t	synth_left_initialized;

#ifdef DUAL_CHANNEL
__attribute__ ((aligned (32)))	MidiSynth_TypeDef Audio_Synth_right =
{
	.status = SYNTH_DISABLED,
	.out_buf = synth_workbuffer_right,
	.out_device = SYNTH_DAC_OUT,
	.codec_buf = dac_buffer_right,
	.sample_rate = SAMPLE_FREQUENCY,
	.wavetable_size = SYNTH_WAVETABLE_1024,
};
uint32_t	synth_right_initialized;
#endif // #ifdef DUAL_CHANNEL

__attribute__ ((aligned (32)))	DAC_Drv_TypeDef DAC_Drv_Left =
{
	.flags = DAC_FLAGS_USE_SYNTHMODULE,
	.dac = &hdac1,
	.dac_timer = &htim6,
	.dac_sample_frequency = SAMPLE_FREQUENCY,
	.dac_buffer = dac_buffer_left,
	.channel = DAC_CHANNEL_1,
	.len = DAC_AUDIO_BUF_SIZE,
};
uint32_t		dac_left_driver_handle;

#ifdef DUAL_CHANNEL
__attribute__ ((aligned (32)))	DAC_Drv_TypeDef DAC_Drv_Right =
{
	.flags = DAC_FLAGS_USE_SYNTHMODULE,
	.dac = &hdac1,
	.dac_timer = &htim6,
	.dac_sample_frequency = SAMPLE_FREQUENCY,
	.dac_buffer = dac_buffer_right,
	.channel = DAC_CHANNEL_2,
	.len = DAC_AUDIO_BUF_SIZE,
};
uint32_t		dac_right_driver_handle;
#endif // #ifdef DUAL_CHANNEL

void	Note(uint8_t channel , uint8_t midi_note , uint8_t midi_velocity)
{
	if (( channel & CMD_MASK ) == 0x90 )
		NoteOn(channel & CHANNEL_MASK,midi_note,midi_velocity);
	else
		NoteOff(channel & CHANNEL_MASK,midi_note);
}

void ControlChange(uint8_t cc_channel,uint8_t cc_index,uint8_t cc_value)
{

}

void ProgramChange(uint8_t pc_index,uint8_t pc_value)
{

}


void PolyPressure(uint8_t midi_channel_status , uint8_t midi_note , uint8_t midi_velocity)
{

}

void PitchBend(uint8_t midi_channel_status , uint8_t midi_note , uint8_t midi_velocity)
{

}

void	SysEx(void)
{

}

uint8_t	rx_sysex_buffer[SYSEX_MAX_LEN];
A_midi_decoder_t	MIDI =
{
	.Note = Note,
	.ControlChange = ControlChange,
	.ProgramChange = ProgramChange,
	.PolyPressure = PolyPressure,
	.PitchBend = PitchBend,
	.SysEx = SysEx,
	.midi_received_sysex_buffer = rx_sysex_buffer,
};
uint32_t	midi_initialized;

void sample_process_1_init(uint32_t process_id)
{
	dac_left_driver_handle = dac_register(&DAC_Drv_Left);
#ifdef DUAL_CHANNEL
	dac_right_driver_handle = dac_register(&DAC_Drv_Right);
#endif // #ifdef DUAL_CHANNEL
	synth_left_initialized = Synth_Register(LEFT_CHANNEL ,&Audio_Synth_left);
#ifdef DUAL_CHANNEL
	synth_right_initialized = Synth_Register(RIGHT_CHANNEL ,&Audio_Synth_right);
#endif // #ifdef DUAL_CHANNEL
	usb_driver_handle = usb_device_driver_register(&USB_Drv);
	midi_initialized = MidiInit(&MIDI);
}

void sample_process_1_audiomidi(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t	cntr = 0;
	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	Synth_Start(&Audio_Synth_left);
#ifdef DUAL_CHANNEL
	Synth_Start(&Audio_Synth_right);
#endif // #ifdef DUAL_CHANNEL

	dac_init(dac_left_driver_handle);
#ifdef DUAL_CHANNEL
	dac_init(dac_right_driver_handle);
#endif // #ifdef DUAL_CHANNEL

	dac_start(dac_left_driver_handle);
#ifdef DUAL_CHANNEL
	dac_start(dac_right_driver_handle);
#endif // #ifdef DUAL_CHANNEL

	while(1)
	{
		wait_event(EVENT_TIMER|EVENT_USB_DEVICE_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			cntr++;
			if ( cntr == 10)
			{
				cntr = 0;
				process_led();
			}
		}
		if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
		{
#ifndef USB_DIRECT_CALLBACK
			if ( midi_initialized == 0 )
				MidiParser(usb_rx_buffer, usb_get_rx_len(usb_driver_handle));
#endif
		}
	}
}
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
#endif // #ifdef SAMPLE_PROCESSES_ENABLED



