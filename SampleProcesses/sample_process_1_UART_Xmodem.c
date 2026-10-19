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
 * sample_process_1_UART_Xmodem.c
 *
 *  Created on: Oct 19, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_UART_XMODEM

#ifdef	STM32H743xx
#define	xmodem_rx_data_area	0x30000000
#define	xmodem_rx_data_len		0x2ffff
#else
#define	xmodem_rx_data_len		0x1ffff
uint8_t	xmodem_rx_data_area[xmodem_rx_data_len];
#endif
extern	UART_HandleTypeDef	huart3;

#define	UART_RX_BUF_SIZE	512
#define	UART_TX_BUF_SIZE	512
uint8_t	uart_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart_tx_buffer[UART_TX_BUF_SIZE];


UART_DriverStruct_t Uart_Drv =
{
	.data = uart_rx_buffer,
	.rx_max_len = XMODEM_LINE_LEN,
	.uart = &huart3,
	.wakeup_id = WAKEUP_FROM_UART3_IRQ,
	.timeout = 100,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
};

uint8_t		xmodem_rx_uart_enable_poll;
uint8_t		tim_downscale=0;
uint32_t	rx_len;

void sample_process_1_uart_xmodem_init(uint32_t process_id)
{
	uart_register(&Uart_Drv);
}

void sample_process_1_uart_xmodem(uint32_t process_id)
{
uint32_t	wakeup,flags;
	uart_start_receive(&Uart_Drv);

	xmodem_rx_uart_enable_poll = 1;
	xmodem_rx_init((uint8_t *)xmodem_rx_data_area,xmodem_rx_data_len);

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_UART3_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( xmodem_rx_uart_enable_poll == 1 )
			{
				tim_downscale ++;
				if ( tim_downscale > 10 )
				{
					xmodem_data_process((uint32_t *)&Uart_Drv,xmodem_rx_uart_enable_poll,XMODEM_IF_UART,uart_rx_buffer);
					tim_downscale = 0;
				}
			}
			process_led();
		}
		if (( wakeup & WAKEUP_FROM_UART3_IRQ) == WAKEUP_FROM_UART3_IRQ)
		{
			if (( flags & WAKEUP_FLAGS_UART_RX) == WAKEUP_FLAGS_UART_RX )
			{
				if ( uart_rx_buffer[0] != '<')
				{
					if ((rx_len = uart_get_rxlen(&Uart_Drv)) > 1 )
						xmodem_rx_uart_enable_poll = 0;
				}
				if ( xmodem_rx_uart_enable_poll == 0 )
					if ( xmodem_data_process((uint32_t *)&Uart_Drv,xmodem_rx_uart_enable_poll,XMODEM_IF_UART,uart_rx_buffer) == X_EOT)
						xmodem_rx_uart_enable_poll = 1;
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_UART_XMODEM
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

