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
 * sample_process_1_sdcard_wav_reader.c
 *
 *  Created on: Oct 19, 2026
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"

#ifdef SAMPLEPROCESS_1_SDCARD_WAV_READER

#include "ff.h"
#include "fatfs.h"

extern	DAC_HandleTypeDef hdac1;
extern	TIM_HandleTypeDef htim7;

#define AUDIO_BUF_SIZE 4096
__attribute__ ((aligned (32))) uint16_t	dac_buffer[AUDIO_BUF_SIZE];
uint16_t audio_buffer[AUDIO_BUF_SIZE] __attribute__((section(".RAM_D1")));

__attribute__ ((aligned (32)))	DAC_DriverStruct_t DAC_Drv =
{
	.dac = &hdac1,
	.dac_timer = &htim7,
	.sample_rate = DEFAULT_SAMPLE_FREQUENCY,
	.dac_buffer = audio_buffer,
	.volume = 0.5F,
	.channel = DAC_CHANNEL_1,
	.dac_buf_size = AUDIO_BUF_SIZE,
	.flags = DAC_FLAGS_WAKEUP,
};

FIL		wav_file;
uint8_t isPlaying= 0 , sdMounted;
FATFS	USERFATFS;					/* File system object for USER logical drive */
char	USERPath[4]; 				/* USER logical drive path */
char	wav_filename[32];

void sample_process_1_sdcard_wav_reader_init(uint32_t process_id)
{
	dac_register(&DAC_Drv);
}

void sample_process_1_sdcard_wav_reader(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t tim_cntr=0;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	dac_init(&DAC_Drv);
	sprintf(wav_filename,"/ALARM/s1k22050.wav");
    if ( f_mount(&SDFatFS, (TCHAR const*)SDPath, 1) == 0 )
    {
    	sdMounted = 1;
    }

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_DAC_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			tim_cntr++;
			if ( tim_cntr == 5 )
			{
				if ( sdMounted )
					if ( WAV_Player_Init(&DAC_Drv,wav_filename) == 0 )
						isPlaying = 1;
			}
			if ( tim_cntr == 7 )
			{
				tim_cntr = 6;
			}
		}
		if (( wakeup & WAKEUP_FROM_DAC_IRQ) == WAKEUP_FROM_DAC_IRQ)
		{
		    if (isPlaying)
		    {
		        WAV_Player_Process(&DAC_Drv);
		    }
		}
	}
}

#endif //#ifdef SAMPLEPROCESS_1_SDCARD_WAV_READER
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

