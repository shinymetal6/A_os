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
 * sample_process_1_xmodem_rx_UART.c
 *
 *  Created on: Jul 23, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_XMODEM_RX_UART

#define	xmodem_rx_data_area	0x30001000
#define	xmodem_rx_data_len		0x2ffff

extern	UART_HandleTypeDef	huart4;

#define	UART_RX_BUF_SIZE	512
#define	UART_TX_BUF_SIZE	512
uint8_t	uart4_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart4_tx_buffer[UART_TX_BUF_SIZE];

UART_Drv_TypeDef Uart4_Drv =
{
	.data = uart4_rx_buffer,
	.rx_max_len = XMODEM_LINE_LEN+1,
	.uart = &huart4,
	.wakeup_id = WAKEUP_FROM_UART4_IRQ,
	.timeout = 50,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
};

uint32_t	uart4_driver_handle;
uint8_t		xmodem_rx_uart_enable_poll;
uint8_t		tim_downscale=0;

void sample_process_1_xmodem_rx_UART(uint32_t process_id)
{
uint32_t	wakeup,flags;
	uart4_driver_handle = uart_register(&Uart4_Drv);
	uart_start_receive(uart4_driver_handle);
	xmodem_rx_uart_enable_poll = 1;

	xmodem_rx_init((uint8_t *)xmodem_rx_data_area,xmodem_rx_data_len);

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_UART4_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( xmodem_rx_uart_enable_poll == 1 )
			{
				tim_downscale ++;
				if ( tim_downscale > 100 )
				{
					xmodem_data_process(xmodem_rx_uart_enable_poll,XMODEM_IF_UART,uart4_driver_handle,uart4_rx_buffer);
					tim_downscale = 0;
				}
			}
			process_led();
		}
		if (( wakeup & WAKEUP_FROM_UART4_IRQ) == WAKEUP_FROM_UART4_IRQ)
		{
			if (( flags & WAKEUP_FLAGS_UART_RX) == WAKEUP_FLAGS_UART_RX )
			{
				if ( uart_get_rxlen(uart4_driver_handle) > 1 )
					xmodem_rx_uart_enable_poll = 0;
				if ( xmodem_rx_uart_enable_poll == 0 )
					if ( xmodem_data_process(xmodem_rx_uart_enable_poll,XMODEM_IF_UART,uart4_driver_handle,uart4_rx_buffer) == X_EOT)
						xmodem_rx_uart_enable_poll = 1;
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_XMODEM_RX_UART
#endif // #ifdef SAMPLE_PROCESSES_ENABLED




