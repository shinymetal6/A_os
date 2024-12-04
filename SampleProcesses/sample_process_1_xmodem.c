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
 * sample_process_1_xmodem.c
 *
 *  Created on: Dec 4, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef 	LD1_GPIO_Port

#define	xmodem_data_area	0x30001000
#define	xmodem_data_len		0x2ffff

extern	UART_HandleTypeDef	huart3;

#define	UART_RX_BUF_SIZE	512
#define	UART_TX_BUF_SIZE	512
uint8_t	uart3_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart3_tx_buffer[UART_TX_BUF_SIZE];

UART_Drv_TypeDef Uart3_Drv =
{
	.data = uart3_rx_buffer,
	.rx_max_len = XMODEM_LINE_LEN,
	.uart = &huart3,
	.wakeup_id = WAKEUP_FROM_UART3_IRQ,
	.timeout = 250,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
};

uint32_t	uart3_driver_handle;
uint8_t		xmodem_uart_reply;
uint8_t		xmodem_uart_enable_poll;

uint8_t		nak=X_NAK,ack=X_ACK;

void sample_process_1_xmodem(uint32_t process_id)
{
uint32_t	wakeup,flags;

	uart3_driver_handle = uart_register(&Uart3_Drv);
	uart_start_receive(uart3_driver_handle);
	xmodem_uart_enable_poll = 1;

	xmodem_init((uint8_t *)xmodem_data_area,xmodem_data_len);

	create_timer(TIMER_ID_0,1000,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_UART3_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( xmodem_uart_enable_poll == 1 )
			{
				xmodem_set_data_area((uint8_t *)xmodem_data_area,xmodem_data_len );
				uart_send(uart3_driver_handle,&nak,1);
			}
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		}
		if (( wakeup & WAKEUP_FROM_UART3_IRQ) == WAKEUP_FROM_UART3_IRQ)
		{
			if (( flags & WAKEUP_FLAGS_UART_RX) == WAKEUP_FLAGS_UART_RX )
			{
				xmodem_uart_enable_poll = 0;
				xmodem_uart_reply = xmodem_line_parser(uart3_rx_buffer);
				switch(xmodem_uart_reply)
				{
				case	X_NAK:
					uart_send(uart3_driver_handle,&nak,1);
					break;
				case	X_EOT:
					uart_send(uart3_driver_handle,&ack,1);
					xmodem_uart_enable_poll = 1;
					break;
				case	X_ACK:
					uart_send(uart3_driver_handle,&ack,1);
					break;
				default:
					uart_send(uart3_driver_handle,&nak,1);
					break;
				}
			}
		}
	}
}
#else
void sample_process_1_xmodem(uint32_t process_id)
{
	wait_event(HW_SLEEP_FOREVER);
}
#endif // #ifdef 	LD1_GPIO_Port

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

