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
 * sample_process_1_audio_I2S_drum.c
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIO_I2S_DRUM

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

AUDIO_FAST_RAM uint16_t left_tx_buffer[I2S_BUFFER_SIZE];
AUDIO_FAST_RAM uint16_t right_tx_buffer[I2S_BUFFER_SIZE];
AUDIO_FAST_RAM uint16_t left_rx_buffer[I2S_BUFFER_SIZE];
AUDIO_FAST_RAM uint16_t right_rx_buffer[I2S_BUFFER_SIZE];

__attribute__ ((aligned (32)))	Nau88C22_Drv_TypeDef	Nau88C22_Drv =
{
	.bus = &hi2c1,
	.device_address = NAU88C22_I2C_ADDR,
	.master_volume = 100,
};

__attribute__ ((aligned (32)))	uint16_t	adc1_buf[ADC1_CHANNELS];
__attribute__ ((aligned (32))) ADC_DriverStruct_t	ADC1_Drv =
{
	.adc = &hadc1,
	.adc_timer = &htim6,
	.adc_buffer = adc1_buf,
	.num_channels = ADC1_CHANNELS,
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

DrumVoice_TypeDef drum_voices[] =
{
    { TR_808_boom_kick,   	TR_808_boom_kick_length,0, 1.0f, 0 }, // Boom Kick
    { TR_808_snare,  		TR_808_snare_length,  	0, 0.9f, 0 }, // Snare
    { TR_808_hh,     		TR_808_hh_length,     	0, 0.7f, 0 }, // Hi-hat
    { TR_808_clap,   		TR_808_clap_length,   	0, 0.8f, 0 },  // Clap
};
// Simple 16-step pattern (1 = trigger)
uint8_t drum_pattern[NUM_VOICES][PATTERN_STEPS] = {
    {1,0,0,0, 1,0,0,0, 1,0,0,0, 1,0,0,0}, // Kick: every beat
    {0,0,1,0, 0,0,1,0, 0,0,1,0, 0,0,1,0}, // Snare: backbeat
    {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1}, // Hi-hat: 16ths
    {0,0,0,0, 0,0,1,0, 0,0,0,0, 0,0,1,0},  // Clap: off-beat
};

DrumMachine_TypeDef	DrumMachine =
{
	.model = TR_808,
	.pattern = (uint8_t *)&drum_pattern,
	.pattern_size = sizeof(drum_pattern),
	.pattern_steps = 4,
	.step_interval = 100,
};

AUDIO_FAST_RAM q15_t	Drum_buf_left[I2S_EFFECT_SIZE];
__attribute__ ((aligned (32)))	AUDIO_Source_TypeDef Audio_Drum_left =
{
	.block_size = I2S_EFFECT_SIZE,
	.out_buf = (q15_t *)Drum_buf_left,
	.flags = SOURCE_ENABLED,
	.sample_rate = SAMPLE_FREQUENCY,
	.ptr_gen_struct = (uint32_t *)&DrumMachine,
};

AUDIO_Dest_TypeDef Out_Port =
{
	.in_buf = (int16_t *)Drum_buf_left,
	.out_buf = (int16_t *)i2s_tx_buffer,
	.out_device = SOURCE_TO_I2S_OUT,
	.flags = SOUND_EFFECT_ENABLED,
};

void sample_process_1_init(uint32_t process_id)
{
	nau88c22_codec_register(&Nau88C22_Drv);
	nau88c22_init(&Nau88C22_Drv);

	i2s_driver_register(&I2S_Driver);
	OutStage_Register(&Out_Port);
	Drum_Machine_Register(&Audio_Drum_left);
	adc_register(&ADC1_Drv);
	adc_start(&ADC1_Drv);
}

void sample_process_1_audio_I2S_drum(uint32_t process_id)
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
			Drum_Machine_Update_sequencer();
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_AUDIO_I2S_DRUM
#endif // #ifdef SAMPLE_PROCESSES_ENABLED



