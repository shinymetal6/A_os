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
 * sample_process_1_pn5180.c
 *
 *  Created on: Jan 16, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED

#include "sample_processes_includes.h"

#ifdef SAMPLEPROCESS_1_PN5180
extern	SPI_HandleTypeDef hspi2;
uint8_t txbuf[PN5180_PAYLOAD_LENGTH];
uint8_t rxbuf[PN5180_PAYLOAD_LENGTH];
uint32_t	nfc_irq=0;
uint32_t nfc_irq_callback(uint16_t GPIO_Pin,uint32_t *param)
{
	nfc_irq++;
	return 0;
}

GPIO_Interrupt_DriverStruct_t	pn5180_Int_Driver;
SPI_NFC_DriverStruct_t	SPI_NFC_Driver =
{
		.wakeup_id = 1,
		//.flags = SPI_USES_DMA,
		.bus = &hspi2,
		.spi_timeout_ms = 100,
		.nfc_model = NFC_IS_PN5180,
		.iso_card = NFC_CARD_IS_14443,
		.tx_data_ptr = txbuf,
		.rx_data_ptr = rxbuf,
		.cs_bit = PN5180_SS_Pin,
		.cs_port = PN5180_SS_GPIO_Port,
		.reset_bit = PN5180_RESET_Pin,
		.reset_port = PN5180_RESET_GPIO_Port,
		.busy_bit = PN5180_BUSY_Pin,
		.busy_port = PN5180_BUSY_GPIO_Port,
		.irq_bit = PN5180_IRQ_Pin,
		.irq_port = PN5180_IRQ_GPIO_Port,
		.IRQ_number = EXTI1_IRQn,
		.nfc_irq_driver = &pn5180_Int_Driver,
		.nfc_irq_callback = nfc_irq_callback,
};
void sample_process_1_init(uint32_t process_id)
{
}

uint8_t	check_val;
uint8_t Key[6] = {0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF};
uint8_t RdBuf[32];

void sample_process_1_pn5180(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	poll_enable = 1 , counter = 0;

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	spi_nfc_register(&SPI_NFC_Driver);
    HAL_GPIO_WritePin(ERROR_GPIO_Port, ERROR_Pin, GPIO_PIN_RESET);
	check_val = rxbuf[0x1a];
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( poll_enable )
			{
			    if ( ISO14443_Discovery(&SPI_NFC_Driver) )
			    {
			    	if ( ISO14443_Authenticate(&SPI_NFC_Driver,Key,NFC_CMD_AUTHA_ISO14443,0) == 0)
			    	{
			    		bzero(RdBuf,sizeof(RdBuf));
			    		ISO14443_BlockRead(&SPI_NFC_Driver,4,RdBuf);
						HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			    	}
					counter = 5;
					poll_enable = 0 ;
			    }
			}
			else
			{
				if ( counter )
					counter--;
				else
					poll_enable = 1;
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_PN5180
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

