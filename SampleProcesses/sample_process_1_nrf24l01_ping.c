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
 * sample_process_1_nrf24l01_ping.c
 *
 *  Created on: Sep 30, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_PING_NRF24L01

//#define		RX_MODE		1
#define		TX_MODE		1

#ifdef	STM32L152xE
		extern	SPI_HandleTypeDef hspi2;
#endif // #ifdef	STM32L152xE
#ifdef	STM32F446xx
		extern	SPI_HandleTypeDef hspi2;
#endif // #ifdef	STM32L152xE


uint8_t txbuf[NRF24L01_PAYLOAD_LENGTH];
uint8_t rxbuf[NRF24L01_PAYLOAD_LENGTH];
uint8_t Packet_Buf[NRF24L01_PAYLOAD_LENGTH];
uint8_t address[NRF24L01_ADDRESS_LENGTH] = {11,22,33,44,55};

static nrf24l01_Drv_TypeDef	nrf24l01_Drv =
{
		.wakeup_id = 1,
		.MHz = 2420,
		.bps = 0,
		.nrf_address = {11,22,33,44,55},
		.spi = &hspi2,
		.spi_timeout_ms = 100,
		.device_id = 0x01,
		.TX_Buf = txbuf,
		.RX_Buf = rxbuf,
		.CS_bit = SPI2_SS_Pin,
		.CS_port = SPI2_SS_GPIO_Port,
		.CE_bit = NRF_CE_Pin,
		.CE_port = NRF_CE_GPIO_Port,
		.IRQ_bit = NRF24_IRQ_Pin,
		.IRQ_port = NRF24_IRQ_GPIO_Port,
		.IRQ_number = EXTI4_IRQn,
};
uint32_t nrf24l01_handle;

extern	void nrf24l01_handle_dio1_irq(uint16_t GPIO_Pin);

GPIO_Interrupt_DriverStruct_t	nrf24l01_Int_Driver =
{
	.IRQ_bit = NRF24_IRQ_Pin,
	.IRQ_port = NRF24_IRQ_GPIO_Port,
	.IRQ_type = GPIO_INT_TYPE_RISING,
	.flags = GPIO_INT_WAKEUP_ON_EVENT,
	.wakeup_id = WAKEUP_FROM_EXT_INT_IRQ,
	.irq_exti_callback = nrf24l01_handle_dio1_irq,
};

uint8_t irq_txcount=0,irq_rxcount=0;
uint8_t nrf_status=0;
uint8_t nrf_mode=0;
void nrf24l01_handle_dio1_irq(uint16_t GPIO_Pin)
{

}

void sample_process_1_init(uint32_t process_id)
{
	nrf24l01_handle = nrf24l01_register(&nrf24l01_Drv);
	gpio_int_register(&nrf24l01_Int_Driver);
#ifdef TX_MODE
	sprintf((char *)txbuf,"Message from NRF2401L");
#endif // #ifdef TX_MODE
}


#ifdef TX_MODE
uint8_t	tx_result;
#endif // #ifdef TX_MODE

void sample_process_1_ping_nrf24l01(uint32_t process_id)
{
uint32_t	wakeup,flags;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
#ifdef TX_MODE
	create_timer(TIMER_ID_1,1000,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
#else
	nrf24l01_set_rx_address(nrf24l01_handle,address);
#endif // #ifdef TX_MODE

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_EXT_INT_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if (( flags & TIMER_ID_0) == TIMER_ID_0)
			{
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
			}
#ifdef TX_MODE
			if (( flags & TIMER_ID_1) == TIMER_ID_1)
			{
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
				tx_result = nrf24l01_tx(nrf24l01_handle,txbuf,address);
			}
#endif // #ifdef RX_MODE
		}
		if (( wakeup & WAKEUP_FROM_EXT_INT_IRQ) == WAKEUP_FROM_EXT_INT_IRQ)
		{
#ifdef RX_MODE
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
			nrf24l01_rx(nrf24l01_handle,nrf24l01_Drv.RX_Buf);
			bzero(rxbuf,NRF24L01_PAYLOAD_LENGTH);
#endif // #ifdef RX_MODE
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_PING_NRF24L01
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
