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
 * sample_process_1_qspi.c
 *
 *  Created on: Dec 4, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"

#ifdef SAMPLEPROCESS_1_QSPI
#ifdef STM32H7xx_HAL_QSPI_H

extern	QSPI_HandleTypeDef hqspi;
extern	ExtFlash_DriverStruct_t	ExtFlashDriverStruct[MAX_EXTMEM_DRIVERS];

W25Qxx_Drv_TypeDef W25Qxx_Drv =
{
		.qspi_bus = &hqspi,
		.flags = QSPI_USES_DMA,
		.FlashSize = 128,
		.wakeup_id = WAKEUP_FROM_QSPI_IRQ,
};
uint32_t	w25_handle;
#define		DATALEN			1024
#define		DATAOFFSET		8
uint8_t		w25_bufw[DATALEN];
uint8_t		w25_bufr[DATALEN];
uint32_t	qspi_state = 0 , qspi_irqs = 0 , done = 0 , qspi_address = 0 , qspi_len = 0;
uint32_t	qspi_error = 0;

void sample_process_1_qspi(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	i;

	create_timer(TIMER_ID_0,1000,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	w25_handle = w25qxx_register(&W25Qxx_Drv);
	qspi_address = 0+DATAOFFSET;
	qspi_len = DATALEN-DATAOFFSET;
	qspi_error = 0;
	for(i=0;i<DATALEN;i++)
		w25_bufw[i] = 255 - (i & 0xff);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_QSPI_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			switch(qspi_state)
			{
			case 0 :
				qspi_error += qspi_erase_blocks(w25_handle,0,1);
				if (( W25Qxx_Drv.status & QSPI_BUSY ) != QSPI_BUSY )
					qspi_state++;
				break;
			case 1 :
				qspi_error += qspi_read(w25_handle,0,w25_bufr,DATALEN);
				qspi_state++;
				break;
			case 2 :
				qspi_error += qspi_write(w25_handle,qspi_address,&w25_bufw[DATAOFFSET],qspi_len);
				qspi_state++;
				break;
			case 3 :
				qspi_error += qspi_read(w25_handle,qspi_address,w25_bufr,qspi_len);
				qspi_state++;
				break;
			case 4 :
				qspi_state = 3;
				break;
			}
		}
		if (( wakeup & WAKEUP_FROM_QSPI_IRQ) == WAKEUP_FROM_QSPI_IRQ)
		{
			qspi_irqs++;
		}
	}
}
#else
void sample_process_1_qspi(uint32_t process_id)
{
	wait_event(HW_SLEEP_FOREVER);
}

#endif // #ifdef SAMPLEPROCESS_1_QSPI
#endif // #ifdef QSPI_ENABLED
#endif // #ifdef SAMPLE_PROCESSES_ENABLED


