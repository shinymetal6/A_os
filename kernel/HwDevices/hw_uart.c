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
 * hw_uart.c
 *
 *  Created on: Sep 4, 2023
 *      Author: fil
 */


#include "main.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"

extern	UART_HandleTypeDef huart1;

extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	Asys_t			Asys;
extern	HWMngr_queue_t	HwQueues[PERIPHERAL_NUM];

uint32_t	set_uart1_rx_buffer(uint8_t *rx_buf)
{
	if ( HWMngr[HW_UART1].process == Asys.current_process )
	{
		HWMngr[HW_UART1].rx_buf = rx_buf;
		return 0;
	}
	return 255;
}

uint32_t send_uart1(uint8_t *ptr,uint16_t len)
{
	if ( HWMngr[HW_UART1].process == Asys.current_process )
	{
		if ( (queue_insert(&HwQueues[HW_UART1],ptr,len) & HW_MNGR_QUEUE_WAS_EMPTY ) == HW_MNGR_QUEUE_WAS_EMPTY )
		{
			return  HAL_UART_Transmit_IT(&huart1, ptr, len);
		}
	}
	return 255;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
uint8_t		numbuf,*ptr;
uint16_t 	len;
	if ( HWMngr[HW_UART1].status == HWMAN_STATUS_ALLOCATED)
	{
		ptr = queue_extract(&HwQueues[HW_UART1], &numbuf, &len);
		if (  numbuf )
		{
			HAL_UART_Transmit_IT(&huart1, ptr, len);
		}
	}
}
