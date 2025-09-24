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
 * sample_process_1_rx_DMA_UART.c
 *
 *  Created on: Sep 22, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_DMA_UART

extern	UART_HandleTypeDef	huart3;

#define	UART				huart3
#define	UART_WAKEUP			WAKEUP_FROM_UART3_IRQ
#define	UART_EVENT			EVENT_UART3_IRQ

#define	UART_RX_BUF_SIZE	512
#define	UART_TX_BUF_SIZE	512
uint8_t	uart_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart_tx_buffer[UART_TX_BUF_SIZE];

UART_Drv_TypeDef Uart_Drv =
{
	.data = uart_rx_buffer,
	.rx_max_len = XMODEM_LINE_LEN,
	.uart = &UART,
	.wakeup_id = UART_WAKEUP,
	.timeout = 1000,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
	//.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_TIMEOUT,
};

uint32_t	uart_driver_handle;
uint32_t	rx_len;
void sample_process_1_xmodem_rx_UART(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	i;
	uart_driver_handle = uart_register(&Uart_Drv);
	uart_start_receive(uart_driver_handle);

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | UART_EVENT);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
		}
		if (( wakeup & UART_WAKEUP) == UART_WAKEUP)
		{
			if (( flags & WAKEUP_FLAGS_UART_RX) == WAKEUP_FLAGS_UART_RX )
			{
				rx_len = uart_get_rxlen(uart_driver_handle);
				//uart_reinit_on_error(uart_driver_handle);
				for(i=0;i<rx_len;i++)
					uart_tx_buffer[i] = uart_rx_buffer[i];
				uart_send(uart_driver_handle, uart_tx_buffer,rx_len);
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_DMA_UART
#endif // #ifdef SAMPLE_PROCESSES_ENABLED



