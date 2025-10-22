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
 * sample_process_1_audio_I2S_in2out.c
 *
 *  Created on: Oct 22, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIO_I2S_IN2OUT

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
#define	ADC1_CHANNELS	6
#define	ADC2_CHANNELS	3

#define	CODEC_NUM_SAMPLES			512
#define	STEREO_CODEC_NUM_SAMPLES	(CODEC_NUM_SAMPLES*2)
#define	EFFECTS_NUM_SAMPLES			(CODEC_NUM_SAMPLES/2)

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

__attribute__ ((aligned (32)))	Nau88C22_Drv_TypeDef	Nau88C22_Drv =
{
	.bus = &hi2c1,
	.device_address = NAU88C22_I2C_ADDR,
	.master_volume = 100,
};
uint8_t codec_handle;

__attribute__ ((aligned (32))) int16_t	synth0_buf_left[EFFECTS_NUM_SAMPLES];
__attribute__ ((aligned (32)))	AudioSource_TypeDef Audio_I2Sin_left =
{
	.synth_out_buf = i2s_in_buffer,
	.synth_block_size = STEREO_CODEC_NUM_SAMPLES,
	.out_device = SOURCE_I2S_OUT,
	.codec_buf = i2s_out_buffer,
};
uint32_t	audio_i2sin_left_initialized;

__attribute__ ((aligned (32)))	uint16_t	adc1_buf[ADC1_CHANNELS];
__attribute__ ((aligned (32))) ADC_Drv_TypeDef	ADC1_Drv =
{
	.adc = &hadc1,
	.adc_timer = &htim6,
	.adc_buffer = adc1_buf,
	.num_channels = ADC1_CHANNELS,
};
uint8_t	adc1_handle;

__attribute__ ((aligned (32)))	int16_t	vca0_buf_left[EFFECTS_NUM_SAMPLES];
uint16_t			vca0_ampl_left = 32768;
uint16_t			vca0_offset_left = 1000;

VCA_Effect_TypeDef	VCA0_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.effect_in_buf = vca0_buf_left,
	.offset = &vca0_offset_left,
	.amplitude = &adc1_buf[2],
	.flags = SOUND_EFFECT_ENABLED,
};

void sample_process_1_init(uint32_t process_id)
{
	codec_handle = nau88c22_codec_register(&Nau88C22_Drv);
	codec_init(codec_handle);
	audio_i2sin_left_initialized = I2SIn_Register(LEFT_CHANNEL ,&Audio_I2Sin_left);

	i2s_handle = i2s_register(&I2S_Drv);

	i2s_start(i2s_handle);
	adc1_handle = int_adc_register(&ADC1_Drv);
	if ( adc_init(adc1_handle) == 0 )
		adc_start(adc1_handle);
	I2SIn_Start(&Audio_I2Sin_left);

}

void sample_process_1_audio_I2S_in2out(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	//Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA0_Left);
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
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_AUDIO_I2S_IN2OUT
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

