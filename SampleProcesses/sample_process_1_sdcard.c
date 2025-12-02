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
 * sample_process_1_sdcard.c
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_SDCARD

#include "../modules/fat/ff.h"
#include "../drivers/sdcard/sd_diskio.h"
extern	SD_HandleTypeDef hsd1;

SDCARD_DriverStruct_t SDCARD =
{
	.hsd = &hsd1,
	.sd_detect_port = SDMMC1_CD_GPIO_Port,
	.sd_detect_bit = SDMMC1_CD_Pin,
};

FATFS fs;
FIL file;
char path[] = "";
FRESULT res;
uint8_t buffer[512];
UINT br;

void sample_process_1_init(uint32_t process_id)
{
	sdcard_register(&SDCARD);
	FRESULT res = f_mount(&fs, "", 1);
	if ( res == 0 )
	{
		// Open a file
		res = f_open(&file, "FILE.TXT", FA_READ);
		if (res == FR_OK)
		{
			f_read(&file, buffer, sizeof(buffer), &br);
			f_close(&file);
		}
	}
}

void sample_process_1_sdcard(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;


	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER|EVENT_USB_DEVICE_IRQ|EVENT_UART2_IRQ|EVENT_ADC2_IRQ);
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
#endif // #ifdef SAMPLEPROCESS_1_SDCARD

#endif // #ifdef SAMPLEPROCESS_1_ADC

