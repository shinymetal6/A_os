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
//#include "../modules/fat/fat.h"

FATFS fs;
FIL file;
FIL MyFile;
uint32_t file_found = 0;
uint32_t byteswritten, bytesread;                     /* File write/read counts */
uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
uint8_t rtext[100];                                   /* File read buffer */
FRESULT res;                                          /* FatFs function common result code */
extern	char SDPath[4]; 										/* SD card logical drive path */
uint8_t workBuffer[_MAX_SS];
HAL_SD_CardCIDTypedef pCID;
HAL_SD_CardCSDTypedef pCSD;
HAL_SD_CardInfoTypeDef CardInfo;
extern	SD_HandleTypeDef hsd1;

void card_test(void)
{
	res = f_mount(&fs, "", 1);
	if ( res )
		return;
	/*
    if((res = f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer))) != FR_OK)
		return;
		*/
	if((res = f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
		return;
	/*##-5- Write data to the text file ################################*/
	res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

	if((byteswritten == 0) || (res != FR_OK))
		return;
	/*##-6- Close the open text file #################################*/
	f_close(&MyFile);

	/*##-7- Open the text file object with read access ###############*/
	if(f_open(&MyFile, "STM32.TXT", FA_READ) != FR_OK)
		return;
	/*##-8- Read data from the text file ###########################*/
	res = f_read(&MyFile, rtext, sizeof(rtext), (UINT*)&bytesread);

	if((bytesread == 0) || (res != FR_OK))
		return;
	/*##-9- Close the open text file #############################*/
	f_close(&MyFile);

	/*##-10- Compare read data with the expected data ############*/
	if((bytesread != byteswritten))
		return;
	else
		file_found = 1;
}
void sample_process_1_init(uint32_t process_id)
{
	BSP_SD_Init();
	HAL_SD_GetCardCID(&hsd1, &pCID);
	HAL_SD_GetCardCSD(&hsd1, &pCSD);
	BSP_SD_GetCardInfo(&CardInfo);

	card_test();
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

