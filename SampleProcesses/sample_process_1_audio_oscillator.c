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
 * sample_process_1_audio_oscillator.c
 *
 *  Created on: Oct 20, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIO_OSCILLATOR

extern	DAC_HandleTypeDef hdac1;
extern	TIM_HandleTypeDef htim6;

#define	LEFT_CHANNEL		0
#define	RIGHT_CHANNEL		1
#define	SAMPLE_FREQUENCY	96000

#define	DAC_BUF_SIZE		1024
#define	EFFECTS_BUF_SIZE	(DAC_BUF_SIZE/2)

//#define	SWEEP_VCA	1

__attribute__ ((aligned (32))) int16_t	dac_buffer_left[DAC_BUF_SIZE];
__attribute__ ((aligned (32)))	DAC_Drv_TypeDef DAC_Drv_Left =
{
	.flags = DAC_FLAGS_USE_SYNTHMODULE,
	.dac = &hdac1,
	.dac_timer = &htim6,
	.dac_sample_frequency = SAMPLE_FREQUENCY,
	.dac_buffer = dac_buffer_left,
	.channel = DAC_CHANNEL_1,
	.len = DAC_BUF_SIZE,
};
uint32_t		dac_left_driver_handle;

AUDIO_FAST_RAM int16_t	synth0_buf_left[EFFECTS_BUF_SIZE];
__attribute__ ((aligned (32)))	AUDIO_Source_TypeDef Audio_Synth_left =
{
	.block_size = EFFECTS_BUF_SIZE,
	.out_device = SOURCE_TO_DAC_OUT,
	.out_buf = synth0_buf_left,
	.device_out_buf = dac_buffer_left,
	.sample_rate = SAMPLE_FREQUENCY/8,
	.wavetable_size = SYNTH_WAVETABLE_1024,
};
uint32_t	synth_left_initialized;

AUDIO_FAST_RAM int16_t	vca0_buf_left[EFFECTS_BUF_SIZE];
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


AUDIO_FAST_RAM int16_t	vca1_buf_left[EFFECTS_BUF_SIZE];
uint16_t			vca1_ampl_left = 32768;
VCA_Effect_TypeDef	VCA1_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.out_buf = vca1_buf_left,
	.amplitude = &vca1_ampl_left,
	.flags = SOUND_EFFECT_ENABLED,
};

void sample_process_1_init(uint32_t process_id)
{
	dac_left_driver_handle = dac_register(&DAC_Drv_Left);
	synth_left_initialized = Synth_Register(LEFT_CHANNEL ,&Audio_Synth_left);
}

#define STEP	200
#define HLIMIT	(65500 - STEP)
#define LLIMIT	(STEP * 2)

uint8_t		up = 0;

void sample_process_1_audio_oscillator(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;
#ifdef SWEEP_VCA
uint8_t		cntr1 = 0;
#endif // #ifdef SWEEP_VCA

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	Synth_Start(&Audio_Synth_left);
	dac_init(dac_left_driver_handle);
	dac_start(dac_left_driver_handle);
	Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA0_Left);
#ifdef SWEEP_VCA
	//Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA1_Left);
#endif // #ifdef SWEEP_VCA
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
			/*
			cntr1++;
			if ( cntr1 == 100)
			{
				vca1_ampl_left = 0;
			}
			if ( cntr1 == 200)
			{
				vca1_ampl_left = 32768;
				cntr1 = 0;
			}
			*/
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
#endif // #ifdef SAMPLEPROCESS_1_AUDIO_OSCILLATOR
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

