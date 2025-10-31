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
 * sample_process_1_audio_I2S_in_effect_out.c
 *
 *  Created on: Oct 31, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIO_I2S_IN_EFFECT_OUT

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
//#define	EFFECTS_NUM_SAMPLES		I2S_EFFECT_SIZE/2

__attribute__((aligned(32))) uint16_t i2s_tx_buffer[I2S_BUFFER_SIZE];
__attribute__((aligned(32))) uint16_t i2s_rx_buffer[I2S_BUFFER_SIZE];

__attribute__((aligned(32))) uint16_t left_tx_buffer[I2S_EFFECT_SIZE];
__attribute__((aligned(32))) uint16_t right_tx_buffer[I2S_EFFECT_SIZE];
__attribute__((aligned(32))) uint16_t left_rx_buffer[I2S_EFFECT_SIZE];
__attribute__((aligned(32))) uint16_t right_rx_buffer[I2S_EFFECT_SIZE];

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

__attribute__ ((aligned (32)))	AUDIO_Source_TypeDef Audio_I2Sin_left =
{
	.in_buf = (int16_t *)left_rx_buffer,
	.out_buf = (int16_t *)left_tx_buffer,
	.device_out_buf = (int16_t *)i2s_tx_buffer,
	.channel_in = AUDIO_SOURCE_LEFT,
	.channel_out = AUDIO_DESTINATION_LEFT,
	.sample_rate = SAMPLE_FREQUENCY,
};

AUDIO_FAST_RAM int16_t	vca0_buf_left[I2S_EFFECT_SIZE];
uint16_t			vca0_ampl_left = 65535;
uint16_t			vca0_offset = 0;
VCA_Effect_TypeDef	VCA0_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.out_buf = vca0_buf_left,
	.amplitude = &vca0_ampl_left,
	.offset = &vca0_offset,
	.flags = SOUND_EFFECT_ENABLED,
};

AUDIO_FAST_RAM int16_t	iir_buf_left[I2S_EFFECT_SIZE];
uint16_t			cutoffFrequency = 2000;
uint16_t			bandwidth = 600;
IIR_Effect_TypeDef	IIR_Left =
{
	.effect = Effect_IIR,
	.effect_init = Effect_IIR_Init,
	.out_buf = iir_buf_left,
	.cutoffFrequency = &cutoffFrequency,
	.bandwidth = &bandwidth,
	.filterType = IIR_FILTER_BAND_PASS,
	.sample_rate = SAMPLE_FREQUENCY,
	.flags = SOUND_EFFECT_ENABLED,
};


AUDIO_FAST_RAM int16_t	OVERDRIVE_buf_left[I2S_EFFECT_SIZE];
uint16_t			overdrive = 32768;
OVERDRIVE_Effect_TypeDef	OVERDRIVE_Left =
{
	.effect = Effect_Overdrive,
	.effect_init = Effect_Overdrive_Init,
	.out_buf = OVERDRIVE_buf_left,
	.overdrive = &overdrive,
	.flags = SOUND_EFFECT_ENABLED,
};

void sample_process_1_init(uint32_t process_id)
{
	nau88c22_codec_register(&Nau88C22_Drv);
	nau88c22_init(&Nau88C22_Drv);
	bzero(i2s_tx_buffer,I2S_BUFFER_SIZE);
	bzero(i2s_rx_buffer,I2S_BUFFER_SIZE);
	i2s_driver_register(&I2S_Driver);
	I2SIn_Register(&Audio_I2Sin_left);
}

void sample_process_1_audio_I2S_in_effect_out(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;
uint8_t		effect_cntr= 0;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	i2s_driver_start(&I2S_Driver);
	I2SIn_Start(&Audio_I2Sin_left);
	Sound_Insert_Effect((uint32_t *)&Audio_I2Sin_left,(uint32_t *)&VCA0_Left);
	//Sound_Insert_Effect((uint32_t *)&Audio_I2Sin_left,(uint32_t *)&IIR_Left);

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
				effect_cntr++;
			}
			/* for overdrive */
			/*
			switch(effect_cntr)
			{
			case 0	:	OVERDRIVE_Left.flags &= ~SOUND_EFFECT_ENABLED;break;
			case 20	:	OVERDRIVE_Left.flags |=  SOUND_EFFECT_ENABLED;break;
			case 40	:	OVERDRIVE_Left.flags |=  FLAGS_OVERDIVE_ASYMMETRIC;break;
			case 60	:	OVERDRIVE_Left.flags &= ~FLAGS_OVERDIVE_ASYMMETRIC;break;
			case 80	:	OVERDRIVE_Left.flags &= ~SOUND_EFFECT_ENABLED;break;
			case 100:	effect_cntr = 0;break;
			}
			*/
			switch(effect_cntr)
			{
			//case 0	:	IIR_Left.flags &= ~SOUND_EFFECT_ENABLED;break;
			case 20	:	IIR_Left.flags |=  SOUND_EFFECT_ENABLED;break;
			case 40:	effect_cntr = 0;break;
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_AUDIO_I2S_IN_EFFECT_OUT
#endif // #ifdef SAMPLE_PROCESSES_ENABLED


