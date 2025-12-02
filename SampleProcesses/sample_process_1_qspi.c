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
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"

#ifdef SAMPLEPROCESS_1_QSPI
#ifdef STM32H7xx_HAL_QSPI_H

/* */
#define	W25Q128JV_PAGE_SIZE	256
uint8_t man_id, dev_id,capacity;
uint8_t tx_data[W25Q128JV_PAGE_SIZE*4] = "FIL & read DMA & write DMA : Hello from STM32H7 QSPI! This is a test write on 1024 bytes!!!!";
uint8_t rx_data[W25Q128JV_PAGE_SIZE*4];
uint32_t flash_address = 0x000000;  // Start of flash

extern	QSPI_HandleTypeDef hqspi;

QSPI_DriverStruct_t W25Qxx_Drv =
{
	.qspi = &hqspi,
	.wakeup_id = WAKEUP_FROM_QSPI_IRQ,
	.qspi_id = QSPI_25XX,
};

#define		DATALEN			256
#define		DATAOFFSET		0
__attribute__ ((aligned (32)))	uint8_t		w25_bufw[DATALEN*4] = "FIL & read DMA & write DMA on Aos : Hello from STM32H7 QSPI! This is a test write on 256 bytes!!!!";
__attribute__ ((aligned (32)))	uint8_t		w25_bufr[DATALEN*4];

uint8_t qspi_state = 0,qspi_irqs=0 , qspi_errs = 0 , ret_ce_val;
void sample_process_1_init(uint32_t process_id)
{
	qspi_register(&W25Qxx_Drv);
	qspi_reset_chip(&W25Qxx_Drv);
}

void sample_process_1_qspi(uint32_t process_id)
{
uint32_t	wakeup,flags;

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_QSPI_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			switch(qspi_state)
			{
			case 0 :
				qspi_get_id(&W25Qxx_Drv);      // Should be: man_id=0xEF, dev_id=0x40, capacity=0x18 (W25Q128JV)
				qspi_state++;
				break;
			case 1:
				//ret_ce_val = qspi_chip_erase(&W25Qxx_Drv);
				qspi_state++;
			case 2:
				qspi_erase_sectors(&W25Qxx_Drv,flash_address);
				qspi_state++;
			case 3 :
				qspi_read(&W25Qxx_Drv,flash_address, rx_data, W25Q128JV_PAGE_SIZE*4);
				qspi_state++;
				break;
			case 4:
				qspi_write(&W25Qxx_Drv,flash_address, tx_data, W25Q128JV_PAGE_SIZE*4);
				qspi_state++;
				break;
			case 5:
				qspi_read(&W25Qxx_Drv,flash_address, rx_data, W25Q128JV_PAGE_SIZE*4);
				qspi_state++;
				break;
			case 6:
				qspi_memory_map(&W25Qxx_Drv);
				qspi_state++;
				break;
			default :
				qspi_state = 10;
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


