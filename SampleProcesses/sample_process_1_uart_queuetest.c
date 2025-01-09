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
 * sample_process_1_uart_queuetest.c
 *
 *  Created on: Dec 11, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"

#ifdef SAMPLEPROCESS_UARTQUEUES

#define	__EXTERN_UART_HANDLE_	huart2
#define	__UART_WAKEUP_			WAKEUP_FROM_UART2_IRQ
extern	UART_HandleTypeDef		__EXTERN_UART_HANDLE_;

#define	UART_RX_BUF_SIZE	192
#define	UART_TX_BUF_SIZE	18
uint8_t	uart_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart_tx_buffer1[UART_TX_BUF_SIZE];
uint8_t	uart_tx_buffer2[UART_TX_BUF_SIZE];
uint8_t	uart_tx_buffer3[UART_TX_BUF_SIZE];
uint8_t	uart_tx_buffer4[UART_TX_BUF_SIZE];

UART_Drv_TypeDef Uart_Drv =
{
	.data = uart_rx_buffer,
	.rx_max_len = XMODEM_LINE_LEN,
	.uart = &__EXTERN_UART_HANDLE_,
	.wakeup_id = __UART_WAKEUP_,
	.timeout = 250,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
};

uint32_t	uart_driver_handle;
uint8_t bufcntr=0;
//#define	SINGLE_NO_QUEUE	1

void sample_process_1_queuetest(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	i;
	for(i=0;i<UART_TX_BUF_SIZE;i++)
	{
		uart_tx_buffer1[i] = 0x31;
		uart_tx_buffer2[i] = 0x32;
		uart_tx_buffer3[i] = 0x33;
		uart_tx_buffer4[i] = 0x34;
	}
	uart_tx_buffer1[0] = uart_tx_buffer2[0] = uart_tx_buffer3[0] = uart_tx_buffer4[0] = 'S';
	uart_tx_buffer1[UART_TX_BUF_SIZE-3] = uart_tx_buffer2[UART_TX_BUF_SIZE-3] = uart_tx_buffer3[UART_TX_BUF_SIZE-3] = uart_tx_buffer4[UART_TX_BUF_SIZE-3] = 'E';
	uart_tx_buffer1[UART_TX_BUF_SIZE-2] = uart_tx_buffer2[UART_TX_BUF_SIZE-2] = uart_tx_buffer3[UART_TX_BUF_SIZE-2] = uart_tx_buffer4[UART_TX_BUF_SIZE-2] = 0x0d;
	uart_tx_buffer1[UART_TX_BUF_SIZE-1] = uart_tx_buffer2[UART_TX_BUF_SIZE-1] = uart_tx_buffer3[UART_TX_BUF_SIZE-1] = uart_tx_buffer4[UART_TX_BUF_SIZE-1] = 0x0a;

	uart_driver_handle = uart_register(&Uart_Drv);
	uart_start_receive(uart_driver_handle);

	create_timer(TIMER_ID_0,500,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER|__UART_WAKEUP_);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
#ifdef SINGLE_NO_QUEUE
			switch(bufcntr)
			{
			case 0 : uart_send(uart_driver_handle, uart_tx_buffer1,UART_TX_BUF_SIZE); break;
			case 1 : uart_send(uart_driver_handle, uart_tx_buffer2,UART_TX_BUF_SIZE); break;
			case 2 : uart_send(uart_driver_handle, uart_tx_buffer3,UART_TX_BUF_SIZE); break;
			case 3 : uart_send(uart_driver_handle, uart_tx_buffer4,UART_TX_BUF_SIZE); break;
			}
			bufcntr++;
			bufcntr &= 0x03;
#else
			uart_send(uart_driver_handle, uart_tx_buffer1,UART_TX_BUF_SIZE);
			uart_send(uart_driver_handle, uart_tx_buffer2,UART_TX_BUF_SIZE);
			uart_send(uart_driver_handle, uart_tx_buffer3,UART_TX_BUF_SIZE);
			uart_send(uart_driver_handle, uart_tx_buffer4,UART_TX_BUF_SIZE);
#endif
		}
	}
}

#endif // #ifdef SAMPLEPROCESS_UARTQUEUES

#endif // #ifdef SAMPLE_PROCESSES_ENABLED
