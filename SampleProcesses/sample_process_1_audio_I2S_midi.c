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
 * sample_process_1_audio_I2S_midi.c
 *
 *  Created on: Oct 29, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIO_I2S_MIDI

extern	TIM_HandleTypeDef htim6;
extern	I2C_HandleTypeDef hi2c1;
extern	I2S_HandleTypeDef hi2s2;
extern	ADC_HandleTypeDef hadc1;
extern	ADC_HandleTypeDef hadc2;

#define	LEFT_CHANNEL		0
#define	RIGHT_CHANNEL		1

#ifdef SOUND_ENGINE_I2S_ENABLED
	#define	SAMPLE_FREQUENCY	I2S_SAMPLE_FREQUENCY
#else
	#define	SAMPLE_FREQUENCY	48000
#endif

//#define SWEEP_VCA	1

#define	USB_BUF_LEN			64
#define	ADC1_CHANNELS	6
#define	ADC2_CHANNELS	3

#define	EFFECTS_NUM_SAMPLES		I2S_BUFFER_SIZE/2

__attribute__((aligned(32))) uint16_t i2s_tx_buffer[I2S_BUFFER_SIZE*2];
__attribute__((aligned(32))) uint16_t i2s_rx_buffer[I2S_BUFFER_SIZE*2];

__attribute__((aligned(32))) uint16_t left_tx_buffer[I2S_BUFFER_SIZE];
__attribute__((aligned(32))) uint16_t right_tx_buffer[I2S_BUFFER_SIZE];
__attribute__((aligned(32))) uint16_t left_rx_buffer[I2S_BUFFER_SIZE];
__attribute__((aligned(32))) uint16_t right_rx_buffer[I2S_BUFFER_SIZE];

#define	USB_BUF_LEN	64

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

__attribute__ ((aligned (32)))	Nau88C22_Drv_TypeDef	Nau88C22_Drv =
{
	.bus = &hi2c1,
	.device_address = NAU88C22_I2C_ADDR,
	.master_volume = 100,
};

I2S_DriverStruct_t I2S_Driver =
{
	.i2s_rx_buffer = i2s_rx_buffer,
	.i2s_tx_buffer = i2s_tx_buffer,
	.left_rx_buffer = left_rx_buffer,
	.right_rx_buffer = right_rx_buffer,
	.left_tx_buffer = left_tx_buffer,
	.right_tx_buffer = right_tx_buffer,
	.i2s = &hi2s2,
};

__attribute__ ((aligned (32))) int16_t	synth0_work_buf_left[EFFECTS_NUM_SAMPLES];
__attribute__ ((aligned (32)))	AUDIO_Source_TypeDef Audio_Synth_left =
{
	.block_size = I2S_EFFECT_SIZE,
	.in_buf = synth0_work_buf_left,
	.out_device = SOURCE_TO_I2S_OUT,
	.out_buf = (q15_t *)left_tx_buffer,
	.device_out_buf = (int16_t *)i2s_tx_buffer,
	.sample_rate = SAMPLE_FREQUENCY,
};
uint32_t	synth_left_initialized;

__attribute__ ((aligned (32)))	uint16_t	adc1_buf[ADC1_CHANNELS];
__attribute__ ((aligned (32))) ADC_DriverStruct_t	ADC1_Drv =
{
	.adc = &hadc1,
	.adc_timer = &htim6,
	.adc_buffer = adc1_buf,
	.num_channels = ADC1_CHANNELS,
};
uint8_t	adc1_handle;

AUDIO_FAST_RAM int16_t	vca0_buf_left[I2S_EFFECT_SIZE];
uint16_t			vca0_ampl_left = 1000;
uint16_t			vca0_offset = 0;
VCA_Effect_TypeDef	VCA0_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.out_buf = vca0_buf_left,
#ifdef SWEEP_VCA
	.amplitude = &vca0_ampl_left,
#else
	.amplitude = &adc1_buf[2],
#endif
	.offset = &vca0_offset,
	.flags = SOUND_EFFECT_ENABLED,
};

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
	nau88c22_codec_register(&Nau88C22_Drv);
	nau88c22_init(&Nau88C22_Drv);

	i2s_driver_register(&I2S_Driver);

	synth_left_initialized = Synth_Register(LEFT_CHANNEL ,&Audio_Synth_left);
	if ( synth_left_initialized == 0 )
		Synth_Start(&Audio_Synth_left);
	i2s_driver_start(&I2S_Driver);
	adc_register(&ADC1_Drv);
	adc_start(&ADC1_Drv);
	usb_driver_handle = usb_device_driver_register(&USB_Drv);
	midi_initialized = MidiInit(&MIDI);
}

uint8_t		up = 0;

#define STEP	200
#define HLIMIT	(65500 - STEP)
#define LLIMIT	(STEP * 2)

void sample_process_1_audio_I2S_midi(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA0_Left);

	//NoteOn(0,69,127);
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
#ifdef SWEEP_VCA
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
#endif // #ifdef SWEEP_VCA
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
#endif // #ifdef SAMPLEPROCESS_1_AUDIO_I2S_MIDI
#endif // #ifdef SAMPLE_PROCESSES_ENABLED




