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
 * sample_process_1_audio_insert_remove.c
 *
 *  Created on: Oct 21, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_AUDIO_INSERT_REMOVE

extern	DAC_HandleTypeDef hdac1;
extern	TIM_HandleTypeDef htim6;

#define	LEFT_CHANNEL		0
#define	RIGHT_CHANNEL		1
#define	SAMPLE_FREQUENCY	96000

__attribute__ ((aligned (32))) int16_t	dac_buffer_left[DAC_AUDIO_BUF_SIZE];
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

AUDIO_FAST_RAM int16_t	synth0_buf_left[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (32)))	Synth_TypeDef Audio_Synth_left =
{
	.status = SYNTH_DISABLED,
	.synth_out_buf = synth0_buf_left,
	.out_device = SYNTH_DAC_OUT,
	.codec_buf = dac_buffer_left,
	.sample_rate = SAMPLE_FREQUENCY,
	.wavetable_size = SYNTH_WAVETABLE_1024,
};
uint32_t	synth_left_initialized;


AUDIO_FAST_RAM int16_t	vca0_buf_left[HALF_NUMBER_OF_AUDIO_SAMPLES];
uint16_t			vca0_ampl_left = 4096;
VCA_Effect_TypeDef	VCA0_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.effect_in_buf = vca0_buf_left,
	.amplitude = &vca0_ampl_left,
	.flags = SOUND_EFFECT_ENABLED,
};

AUDIO_FAST_RAM int16_t	vca1_buf_left[HALF_NUMBER_OF_AUDIO_SAMPLES];
uint16_t			vca1_ampl_left = 8192;
VCA_Effect_TypeDef	VCA1_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.effect_in_buf = vca1_buf_left,
	.amplitude = &vca1_ampl_left,
	.flags = SOUND_EFFECT_ENABLED,
};

AUDIO_FAST_RAM int16_t	vca2_buf_left[HALF_NUMBER_OF_AUDIO_SAMPLES];
uint16_t			vca2_ampl_left = 16384;
VCA_Effect_TypeDef	VCA2_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.effect_in_buf = vca2_buf_left,
	.amplitude = &vca2_ampl_left,
	.flags = SOUND_EFFECT_ENABLED,
};

AUDIO_FAST_RAM int16_t	vca3_buf_left[HALF_NUMBER_OF_AUDIO_SAMPLES];
uint16_t			vca3_ampl_left = 32768;
VCA_Effect_TypeDef	VCA3_Left =
{
	.effect = Effect_VCA,
	.effect_init = Effect_VCA_Init,
	.effect_in_buf = vca3_buf_left,
	.amplitude = &vca3_ampl_left,
	.flags = SOUND_EFFECT_ENABLED,
};

void sample_process_1_init(uint32_t process_id)
{
	dac_left_driver_handle = dac_register(&DAC_Drv_Left);
	synth_left_initialized = Synth_Register(LEFT_CHANNEL ,&Audio_Synth_left);
}

uint32_t		cntr1 = 0;
void sample_process_1_audio_insert_remove(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	Synth_Start(&Audio_Synth_left);
	dac_init(dac_left_driver_handle);
	dac_start(dac_left_driver_handle);
	Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA0_Left);
	Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA1_Left);
	Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA2_Left);
	Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA3_Left);
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
			switch(cntr1)
			{
			case 100 :		Sound_Remove_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA3_Left);cntr1++; break;
			case 200 :		Sound_Remove_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA2_Left);cntr1++; break;
			case 300 :		Sound_Remove_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA1_Left);cntr1++; break;
			case 400 :		Sound_Remove_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA0_Left);cntr1++; break;
			case 500 :		Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA0_Left);cntr1++; break;
			case 600 :		Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA1_Left);cntr1++; break;
			case 700 :		Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA2_Left);cntr1++; break;
			case 800 :		Sound_Insert_Effect((uint32_t *)&Audio_Synth_left,(uint32_t *)&VCA3_Left);cntr1++; break;
			default:
				cntr1++;
				if ( cntr1 == 900)
					cntr1 = 0;
				break;
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_AUDIO_INSERT_REMOVE
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
