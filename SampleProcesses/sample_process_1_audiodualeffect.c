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
 * sample_process_1_audiodualeffect.c
 *
 *  Created on: Oct 17, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIODUAL_EFFECT

extern	DAC_HandleTypeDef hdac1;
extern	TIM_HandleTypeDef htim6;

#define	USB_BUF_LEN	64

#define	DUAL_CHANNEL	1

__attribute__ ((aligned (32))) int16_t	dac_buffer_left[DAC_AUDIO_BUF_SIZE*2];
__attribute__ ((aligned (32))) int16_t	synth_workbuffer_left[NUMBER_OF_AUDIO_SAMPLES];

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


void	Note(uint8_t channel , uint8_t midi_note , uint8_t midi_velocity)
{
	if (( channel & CMD_MASK ) == 0x90 )
		NoteOn(channel & CHANNEL_MASK,midi_note,midi_velocity);
	else
		NoteOff(channel & CHANNEL_MASK,midi_note);
}

A_midi_decoder_t	MIDI =
{
	.Note = Note,
};
uint32_t	midi_initialized;

/*******************/
/****	Effects ****/
/*******************/
#define VCA_EFFECT	1
#define OVERDRIVE_EFFECT	1
#define PHASER_EFFECT		1

#ifdef VCA_EFFECT
uint16_t	vca0_ampl_left = 0;
uint8_t		up = 1;

VCA_Effect_TypeDef	VCA0_Effect_Left =
{
	.amplitude = &vca0_ampl_left,
	.flags = SOUND_EFFECT_ENABLED,
};
__attribute__ ((aligned (32))) int16_t	vca0_buf_left[HALF_NUMBER_OF_AUDIO_SAMPLES];
Effect_TypeDef	VCA0_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.in_buf = vca0_buf_left,
	.private_data = (uint32_t *)&VCA0_Effect_Left,
	.status = SOUND_EFFECT_AUTO_ENABLE,
};
#endif // #ifdef VCA_EFFECT

#ifdef PHASER_EFFECT
uint16_t	lfo_rate = 50,depth = 50,mix = 50;
PHASER_Effect_TypeDef	PHASER_Effect_left =
{
		.lfo_rate = &lfo_rate,
		.depth = &depth,		// Depth of the LFO
		.mix = &mix,
		.flags = SOUND_EFFECT_ENABLED,
};

__attribute__ ((aligned (32))) int16_t	phaser_buf_left[HALF_NUMBER_OF_AUDIO_SAMPLES];
Effect_TypeDef	PHASER_Left =
{
	.effect = Effect_Phaser,
	.effect_init = Effect_Phaser_Init,
	.in_buf = phaser_buf_left,
	.private_data = (uint32_t *)&PHASER_Effect_left,
	.status = SOUND_EFFECT_AUTO_ENABLE,
};
#endif // #ifdef PHASER_EFFECT

#ifdef OVERDRIVE_EFFECT
uint8_t		overdrive_cntr = 0;
uint16_t		overdrive = 3;

OVERDRIVE_Effect_TypeDef	OVERDRIVE_Effect_Left =
{
		.overdrive = &overdrive,
		.flags = SOUND_EFFECT_ENABLED,
};

__attribute__ ((aligned (32))) int16_t	overdrive_buf_left[HALF_NUMBER_OF_AUDIO_SAMPLES];
Effect_TypeDef	OVERDRIVE_Left =
{
	.effect_init = Effect_Overdrive_Init,
	.effect = Effect_Overdrive,
	.in_buf = overdrive_buf_left,
	.private_data = (uint32_t *)&OVERDRIVE_Effect_Left,
	.status = SOUND_EFFECT_AUTO_ENABLE,
};
#endif // #ifdef OVERDRIVE_EFFECT

void sample_process_1_init(uint32_t process_id)
{
	dac_left_driver_handle = dac_register(&DAC_Drv_Left);
	synth_left_initialized = Synth_Register(LEFT_CHANNEL ,&Audio_Synth_left);
	usb_driver_handle = usb_device_driver_register(&USB_Drv);
	midi_initialized = MidiInit(&MIDI);
}

#define STEP	200
#define HLIMIT	(65500 - STEP)
#define LLIMIT	(STEP * 2)
void sample_process_1_audiodualeffect(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	Synth_Start(&Audio_Synth_left);
	dac_init(dac_left_driver_handle);
	dac_start(dac_left_driver_handle);
#ifdef VCA_EFFECT
	Sound_Insert_Effect(&Audio_Synth_left,&VCA0_Left);
#endif // #ifdef VCA_EFFECT
#ifdef PHASER_EFFECT
	Sound_Insert_Effect(&Audio_Synth_left,&PHASER_Left);
#endif // #ifdef PHASER_EFFECT
#ifdef OVERDRIVE_EFFECT
	Sound_Insert_Effect(&Audio_Synth_left,&OVERDRIVE_Left);
#endif // #ifdef OVERDRIVE_EFFECT

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
#ifdef VCA_EFFECT
			if ( up )
			{
				vca0_ampl_left += STEP;
				if ( vca0_ampl_left > HLIMIT )
					up = 0;
			}
			else
			{
				vca0_ampl_left -= STEP;
				if ( vca0_ampl_left < LLIMIT )
					up = 1;
			}
#endif // #ifdef VCA_EFFECT
#ifdef OVERDRIVE_EFFECT
			overdrive_cntr++;
			if ( overdrive_cntr == 100)
			{
				OVERDRIVE_Effect_Left.flags |= SOUND_EFFECT_ENABLED;
			}
			if ( overdrive_cntr == 200)
			{
				OVERDRIVE_Effect_Left.flags &= ~SOUND_EFFECT_ENABLED;
				overdrive_cntr = 0;
			}
#endif // #ifdef OVERDRIVE_EFFECT

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
#endif // #ifdef SAMPLEPROCESS_1_AUDIODUAL_EFFECT
#endif // #ifdef SAMPLE_PROCESSES_ENABLED





