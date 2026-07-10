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
 * sample_process_1_dfplayer.c
 *
 *  Created on: Jul 8, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_DFPLAYER

extern	UART_HandleTypeDef	huart1;
#define	UART_RX_BUF_SIZE	32
#define	UART_TX_BUF_SIZE	64
uint8_t	uart_rx_buffer[UART_RX_BUF_SIZE];
uint8_t	uart_tx_buffer[UART_TX_BUF_SIZE];

UART_DriverStruct_t Uart1_MP3Player_Drv =
{
	.data = uart_rx_buffer,
	.rx_max_len = UART_RX_BUF_SIZE,
	.uart = &huart1,
	.wakeup_id = WAKEUP_FROM_UART1_IRQ,
	.timeout = 100,
	.flags = UART_USES_DMA_TX | UART_USES_DMA_RX | UART_WAKEUP_ON_RXFULL | UART_WAKEUP_ON_TIMEOUT,
};
MODULES_DFPlayer_Struct_t	MODULE_DFPlayer =
{
	.uart_drv = &Uart1_MP3Player_Drv,
	.DfPlayer_rxbuffer_len = UART_RX_BUF_SIZE,
	.DfPlayer_query_tracks_command = DFPLAYER_CMD_QUERY_TOTAL_TRACKS48,
	.busy_bit = MP3_BUSY_Pin,
	.busy_port = MP3_BUSY_GPIO_Port,
};

void sample_process_1_dfplayer(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	dfcount=0;

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	uart_register(&Uart1_MP3Player_Drv);
	DfPlayer_register(&MODULE_DFPlayer);
    uart_start_receive(MODULE_DFPlayer.uart_drv);
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_UART1_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			if ( DfPlayer_state_machine(&MODULE_DFPlayer) == DFPLAYER_SM_MAINLOOP)
			{
				if ( dfcount == 0 )
					DfPlayer_command(&MODULE_DFPlayer,DFPLAYER_CMD_PLAY);
				if ( dfcount == 20 )
					DfPlayer_command(&MODULE_DFPlayer,DFPLAYER_CMD_STOP);
				if ( dfcount >= 30 )
					dfcount = 0;
				else
					dfcount ++;
			}
		}
		if (( wakeup & WAKEUP_FROM_UART1_IRQ) == WAKEUP_FROM_UART1_IRQ)
		{
			if (( flags & WAKEUP_FLAGS_UART_RX) == WAKEUP_FLAGS_UART_RX)
				DFPlayer_ProcessResponse(&MODULE_DFPlayer,uart_get_rxlen(MODULE_DFPlayer.uart_drv));
		}
	}
}

#endif //#ifdef SAMPLEPROCESS_1_DFPLAYER
#endif //#ifdef SAMPLE_PROCESSES_ENABLED

