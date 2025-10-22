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
 * sample_process_1_audio_I2S_oscillator.c
 *
 *  Created on: Oct 22, 2025
 *      Author: fil
 */
/*
Description:
I2S Example with direct connection of ad channel to volume ( see .amplitude = &adc1_buf[2] in VCA0_Left struct init )
The note generated is 440Hz.
  */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIO_I2S_OSCILLATOR

extern	TIM_HandleTypeDef htim6;
extern	I2C_HandleTypeDef hi2c1;
extern	I2S_HandleTypeDef hi2s2;
extern	ADC_HandleTypeDef hadc1;
extern	ADC_HandleTypeDef hadc2;

#define	LEFT_CHANNEL		0
#define	RIGHT_CHANNEL		1

#ifdef SOUND_ENGINE_I2S_ENABLED
	#define	SAMPLE_FREQUENCY	USER_I2S_SAMPLE_FREQUENCY
#else
	#define	SAMPLE_FREQUENCY	48000
#endif

#define	USB_BUF_LEN			64
#define	ADC1_CHANNELS	6
#define	ADC2_CHANNELS	3

#define	CODEC_NUM_SAMPLES			512
#define	STEREO_CODEC_NUM_SAMPLES	(CODEC_NUM_SAMPLES*2)
#define	EFFECTS_NUM_SAMPLES			(CODEC_NUM_SAMPLES/2)

__attribute__ ((aligned (32)))	uint8_t		usb_rx_buffer[USB_BUF_LEN];
__attribute__ ((aligned (32)))	uint8_t		usb_tx_buffer[USB_BUF_LEN];

__attribute__ ((aligned (32))) int16_t		i2s_in_buffer[STEREO_CODEC_NUM_SAMPLES];
__attribute__ ((aligned (32))) int16_t		i2s_out_buffer[STEREO_CODEC_NUM_SAMPLES];
__attribute__ ((aligned (32)))	I2S_Drv_TypeDef	I2S_Drv =
{
	.i2s = 	&hi2s2,
	.i2s_in_buffer = i2s_in_buffer,
	.i2s_out_buffer = i2s_out_buffer,
	.len = STEREO_CODEC_NUM_SAMPLES,
	.flags = I2S_FLAGS_USE_SYNTHMODULE,
	.wakeup_id = EVENT_I2S2_IRQ,
};
uint8_t i2s_handle;

__attribute__ ((aligned (32))) int16_t	synth0_buf_left[EFFECTS_NUM_SAMPLES];
__attribute__ ((aligned (32)))	Synth_TypeDef Audio_Synth_left =
{
	.status = SYNTH_DISABLED,
	.synth_out_buf = synth0_buf_left,
	.synth_block_size = EFFECTS_NUM_SAMPLES,
	.out_device = SYNTH_I2S_OUT,
	.codec_buf = i2s_out_buffer,
	.sample_rate = SAMPLE_FREQUENCY,
	.wavetable_size = SYNTH_WAVETABLE_1024,
};
uint32_t	synth_left_initialized;

__attribute__ ((aligned (32)))	Nau88C22_Drv_TypeDef	Nau88C22_Drv =
{
	.bus = &hi2c1,
	.device_address = NAU88C22_I2C_ADDR,
	.master_volume = 100,
};
uint8_t codec_handle;

__attribute__ ((aligned (32)))	uint16_t	adc1_buf[ADC1_CHANNELS];
__attribute__ ((aligned (32))) ADC_Drv_TypeDef	ADC1_Drv =
{
	.adc = &hadc1,
	.adc_timer = &htim6,
	.adc_buffer = adc1_buf,
	.num_channels = ADC1_CHANNELS,
};
uint8_t	adc1_handle;

__attribute__ ((aligned (32)))	uint16_t	adc2_buf[ADC2_CHANNELS];
__attribute__ ((aligned (32))) ADC_Drv_TypeDef	ADC2_Drv =
{
	.adc = &hadc2,
	.adc_timer = &htim6,
	.adc_buffer = adc2_buf,
	.num_channels = ADC2_CHANNELS,
};
uint8_t	adc2_handle;


__attribute__ ((aligned (32)))	int16_t	vca0_buf_left[EFFECTS_NUM_SAMPLES];
uint16_t			vca0_ampl_left = 32768;
#ifndef SWEEP_VCA
uint16_t			vca0_offset_left = 1000;
#endif // #ifndef SWEEP_VCA

VCA_Effect_TypeDef	VCA0_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.effect_in_buf = vca0_buf_left,
#ifdef SWEEP_VCA
	.amplitude = &vca0_ampl_left,
	.offset = &vca0_offset_left,
#else
	.amplitude = &adc1_buf[2],
#endif // #ifdef SWEEP_VCA
	.flags = SOUND_EFFECT_ENABLED,
};

void sample_process_1_init(uint32_t process_id)
{
	codec_handle = nau88c22_codec_register(&Nau88C22_Drv);
	codec_init(codec_handle);

	i2s_handle = i2s_register(&I2S_Drv);

	synth_left_initialized = Synth_Register(LEFT_CHANNEL ,&Audio_Synth_left);
	Audio_Synth_left.i2s_handle = i2s_handle;
	if ( synth_left_initialized == 0 )
		Synth_Start(&Audio_Synth_left);
	i2s_start(i2s_handle);
	adc1_handle = int_adc_register(&ADC1_Drv);
	adc2_handle = int_adc_register(&ADC2_Drv);
	if ( adc_init(adc1_handle) == 0 )
		adc_start(adc1_handle);
	if ( adc_init(adc2_handle) == 0 )
		adc_start(adc2_handle);
}

uint8_t		up = 0;

#define STEP	200
#define HLIMIT	(65500 - STEP)
#define LLIMIT	(STEP * 2)

void sample_process_1_audio_I2S_oscillator(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA0_Left);
	NoteOn(0,69,127);
	while(1)
	{
		wait_event(EVENT_TIMER);
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
	}
}
#endif // #ifdef SAMPLEPROCESS_1_AUDIO_I2S_OSCILLATOR
#endif // #ifdef SAMPLE_PROCESSES_ENABLED


