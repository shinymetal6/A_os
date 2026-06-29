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
 * sample_process_1_hc05.c
 *
 *  Created on: Jun 29, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_HC05

extern	UART_HandleTypeDef	huart8;
#define	UART				huart8
#define	UART_WAKEUP			WAKEUP_FROM_UART8_IRQ
#define	UART_EVENT			EVENT_UART8_IRQ

#define	UART_RX_BUF_SIZE	512
#define	UART_TX_BUF_SIZE	512
uint8_t	uart_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart_tx_buffer[UART_TX_BUF_SIZE];

UART_DriverStruct_t Uart_Drv =
{
	.data = uart_rx_buffer,
	.rx_max_len = UART_RX_BUF_SIZE/4,
	.uart = &UART,
	.wakeup_id = UART_WAKEUP,
	.timeout = 100,
	//.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
	.flags = UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
};
uint32_t	rxed=0;
void sample_process_1_hc05(uint32_t process_id)
{
uint32_t	wakeup,flags;

	uart_register(&Uart_Drv);
	uart_start_receive(&Uart_Drv);
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_UART8_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
		}
		if (( wakeup & EVENT_UART8_IRQ) == EVENT_UART8_IRQ)
		{
			rxed = uart_get_rxlen(&Uart_Drv);
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_ADC
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
