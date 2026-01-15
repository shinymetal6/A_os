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
 * sample_process_1_nrf24l01_pong.c
 *
 *  Created on: Sep 30, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_PONG_NRF24L01

extern	SPI_HandleTypeDef hspi2;

uint8_t txbuf[NRF24L01_PAYLOAD_LENGTH];
uint8_t rxbuf[NRF24L01_PAYLOAD_LENGTH];

uint32_t nrf24l01_irq_callback(uint16_t GPIO_Pin,uint32_t *param)
{
	return 0;
}
GPIO_Interrupt_DriverStruct_t	nrf24l01_Int_Driver;
static SPI_NRF24L01_DriverStruct_t	nrf24l01_Drv =
{
		.wakeup_id = 1,
		.MHz = 2420,
		.bps = 0,
		.nrf_tx_address = {0xB3,0xB4,0xB5,0xB6,0x05},
		.nrf_rx_address = {0xB3,0xB4,0xB5,0xB6,0x05},
		.bus = &hspi2,
		.spi_timeout_ms = 100,
		.device_id = 0x01,
		.TX_Buf = txbuf,
		.RX_Buf = rxbuf,
		.cs_bit = SPI2_SS_Pin,
		.cs_port = SPI2_SS_GPIO_Port,
		.ce_bit = NRF_CE_Pin,
		.ce_port = NRF_CE_GPIO_Port,
		.irq_bit = NRF24_IRQ_Pin,
		.irq_port = NRF24_IRQ_GPIO_Port,
		.IRQ_number = EXTI4_IRQn,
		.nrf24l01_irq_driver = &nrf24l01_Int_Driver,
		.nrf24l01_irq_callback = nrf24l01_irq_callback,
};

void sample_process_1_pong_init(uint32_t process_id)
{
	spi_nrf24l01_register(&nrf24l01_Drv);
}

uint8_t	tx_result;
int	messageNum = 0;
int	recv_messages = 0;
int	txed_messages = 0;
int	dest_error_messages = 0;
uint32_t status;

void sample_process_1_pong_nrf24l01(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		led_on=0;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	spi_nrf24l01_flush_rx_fifo(&nrf24l01_Drv);
	spi_nrf24l01_flush_tx_fifo(&nrf24l01_Drv);

	sprintf((char *)txbuf,"PONG %d",messageNum);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_EXT_INT_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( led_on )
			{
				led_on = 0;
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
			}
		}
		if (( wakeup & WAKEUP_FROM_EXT_INT_IRQ) == WAKEUP_FROM_EXT_INT_IRQ)
		{
			if ( spi_nrf24l01_check_if_rx(&nrf24l01_Drv))
			{
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
				led_on = 1;
				spi_nrf24l01_get_rx(&nrf24l01_Drv);
				recv_messages++;
				task_delay(1);																	// need to wait at least 130uSec for radio switch
				tx_result = spi_nrf24l01_tx(&nrf24l01_Drv,txbuf,nrf24l01_Drv.nrf_tx_address);
				messageNum++;
				sprintf((char *)txbuf,"PONG %d",messageNum);
			}
			if ( spi_nrf24l01_check_if_maxrt(&nrf24l01_Drv))
			{
				dest_error_messages++;
				spi_nrf24l01_clear_maxrt(&nrf24l01_Drv);
			}
			if ( spi_nrf24l01_check_if_tx(&nrf24l01_Drv))
			{
				txed_messages++;
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_PONG_NRF24L01
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
