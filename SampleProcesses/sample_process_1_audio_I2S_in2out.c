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
	#define	SAMPLE_FREQUENCY	I2S_SAMPLE_FREQUENCY
#else
	#define	SAMPLE_FREQUENCY	48000
#endif
#define	ADC1_CHANNELS	6
#define	ADC2_CHANNELS	3

__attribute__((aligned(32))) uint16_t i2s_tx_buffer[I2S_BUFFER_SIZE*2];
__attribute__((aligned(32))) uint16_t i2s_rx_buffer[I2S_BUFFER_SIZE*2];

__attribute__((aligned(32))) uint16_t left_tx_buffer[I2S_BUFFER_SIZE];
__attribute__((aligned(32))) uint16_t right_tx_buffer[I2S_BUFFER_SIZE];
__attribute__((aligned(32))) uint16_t left_rx_buffer[I2S_BUFFER_SIZE];
__attribute__((aligned(32))) uint16_t right_rx_buffer[I2S_BUFFER_SIZE];

__attribute__ ((aligned (32)))	Nau88C22_Drv_TypeDef	Nau88C22_Drv =
{
	.bus = &hi2c1,
	.device_address = NAU88C22_I2C_ADDR,
	.master_volume = 100,
};
uint8_t codec_handle;

I2S_DriverStruct_t I2S_Driver =
{
		.i2s_rx_buffer = i2s_rx_buffer,
		.i2s_tx_buffer = i2s_tx_buffer,
		.left_rx_buffer = left_rx_buffer,
		.right_rx_buffer = right_rx_buffer,
		.left_tx_buffer = left_tx_buffer,
		.right_tx_buffer = right_tx_buffer,
		.i2s = &hi2s2,
		.flags = I2S_FLAGS_ECHO,
};

void sample_process_1_init(uint32_t process_id)
{
	codec_handle = nau88c22_codec_register(&Nau88C22_Drv);
	codec_init(codec_handle);
	i2s_driver_register(&I2S_Driver);
}

void sample_process_1_audio_I2S_in2out(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	i2s_driver_start(&I2S_Driver);
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

