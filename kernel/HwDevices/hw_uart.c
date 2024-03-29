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
#include "hw_uart.h"

#define	NO_SENTINEL	0

extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	Asys_t			Asys;
uint8_t	rx_char;
extern	HW_Uart_t		HW_Uart[A_MAX_UART];

uint32_t hw_send_uart(uint32_t uart,uint8_t *ptr,uint16_t len)
{
	return  HAL_UART_Transmit_IT(HW_Uart[uart-HW_UART1].hwuart_handle, ptr, len);
}

uint32_t hw_send_uart_dma(uint32_t uart,uint8_t *ptr,uint16_t len)
{
	return HAL_UART_Transmit_DMA(HW_Uart[uart-HW_UART1].hwuart_handle, ptr, len);
}

uint32_t hw_receive_uart(uint32_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint16_t timeout)
{
	if ( HWMngr[uart].process == Asys.current_process )
	{
		HWMngr[uart].sentinel_start = HWMngr[uart].sentinel_end = NO_SENTINEL;
		HWMngr[uart].status &= ~HWMAN_SENTINEL_ENABLE;
		HWMngr[uart].rx_buf = rx_buf;
		HWMngr[uart].rx_buf_max_len = rx_buf_max_len;
		if ( timeout )
		{
			HWMngr[uart].timeout = HWMngr[uart].timeout_reload_value = timeout;
			HWMngr[uart].flags |= HWMAN_TIMEOUT_ENABLED;
		}
		HWMngr[uart].rx_buf_index = 0;
		HAL_UART_Receive_IT(HW_Uart[uart-HW_UART1].hwuart_handle, &rx_char, 1);
		return 0;
	}
	return 255;
}

uint32_t hw_receive_uart_sentinel(uint32_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end,uint16_t timeout)
{
	if ( HWMngr[uart].process == Asys.current_process )
	{
		HWMngr[uart].sentinel_start = sentinel_start;
		HWMngr[uart].sentinel_end = sentinel_end;
		HWMngr[uart].status &= ~HWMAN_SENTINEL_FOUND;
		HWMngr[uart].status |= HWMAN_SENTINEL_ENABLE;

		HWMngr[uart].rx_buf = rx_buf;
		HWMngr[uart].rx_buf_max_len = rx_buf_max_len;
		if ( timeout )
		{
			HWMngr[uart].timeout = HWMngr[uart].timeout_reload_value = timeout;
			HWMngr[uart].flags |= HWMAN_TIMEOUT_ENABLED;
		}
		HWMngr[uart].rx_buf_index = 0;
		HAL_UART_Receive_IT(HW_Uart[uart-HW_UART1].hwuart_handle, &rx_char, 1);
		return 0;
	}
	return 255;
}

uint32_t hw_receive_uart_sentinel_clear(uint32_t uart)
{
	if ( HWMngr[uart].process == Asys.current_process )
	{
		HWMngr[uart].sentinel_start = HWMngr[uart].sentinel_end = NO_SENTINEL;
		return 0;
	}
	return 255;
}

uint16_t hw_get_uart_receive_len(uint32_t uart)
{
	if (( HWMngr[uart].status & HWMAN_STATUS_ALLOCATED) == HWMAN_STATUS_ALLOCATED)
		if ( HWMngr[uart].process == Asys.current_process )
			return HWMngr[uart].rxlen;
	return 0;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
uint32_t	i;
	__disable_irq();
	for(i=HW_UART1;i<HW_UART4;i++)
	{
		if (( HWMngr[i].status & HWMAN_STATUS_ALLOCATED) == HWMAN_STATUS_ALLOCATED)
		{
			activate_process(HWMngr[i].process,1<<i,WAKEUP_FLAGS_UART_TX);
		}
	}
	__enable_irq();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
uint32_t	i;
	__disable_irq();
	for(i=HW_UART1;i<HW_UART4;i++)
	{
		if (( HWMngr[i].status & HWMAN_STATUS_ALLOCATED) == HWMAN_STATUS_ALLOCATED)
		{
			if ( HWMngr[i].rx_buf == NULL )
			{
				HAL_UART_Receive_IT(huart, &rx_char, 1);
				__enable_irq();
				return;
			}
			if (( HWMngr[i].status & HWMAN_SENTINEL_ENABLE) == HWMAN_SENTINEL_ENABLE)
			{
				if (( HWMngr[i].status & HWMAN_SENTINEL_FOUND) != HWMAN_SENTINEL_FOUND)
				{
					if (( rx_char >= HWMngr[i].sentinel_start) && (rx_char <= HWMngr[i].sentinel_end ))
					{
						HWMngr[i].status |= HWMAN_SENTINEL_FOUND;
						HWMngr[i].rx_buf_index=0;
					}
				}

				if (( HWMngr[i].status & HWMAN_SENTINEL_FOUND) == HWMAN_SENTINEL_FOUND)
				{
					HWMngr[i].rx_buf[HWMngr[i].rx_buf_index] = rx_char;
					HWMngr[i].timeout = HWMngr[i].timeout_reload_value;
					HWMngr[i].rx_buf_index++;
					if ( HWMngr[i].rx_buf_index == HWMngr[i].rx_buf_max_len)
					{
						HWMngr[i].rxlen = HWMngr[i].rx_buf_index;
						HWMngr[i].rx_buf_index = 0;
						HWMngr[i].status &= ~HWMAN_SENTINEL_FOUND;
						activate_process(HWMngr[i].process,1<<i,WAKEUP_FLAGS_UART_RX);
					}
				}
			}
			else
			{
				HWMngr[i].rx_buf[HWMngr[i].rx_buf_index] = rx_char;
				HWMngr[i].rx_buf_index++;
				if ( HWMngr[i].rx_buf_index == HWMngr[i].rx_buf_max_len)
				{
					HWMngr[i].rxlen = HWMngr[i].rx_buf_index;
					HWMngr[i].rx_buf_index = 0;
					activate_process(HWMngr[i].process,1<<i,WAKEUP_FLAGS_UART_RX);
				}
			}
		}
	}
	HAL_UART_Receive_IT(huart, &rx_char, 1);
	__enable_irq();
}

void HAL_UART_RxTimeoutCheckCallback(void)
{
uint32_t	i;
	for(i=HW_UART1;i<HW_UART3+1;i++)
	{
		if (( HWMngr[i].status & HWMAN_STATUS_ALLOCATED) == HWMAN_STATUS_ALLOCATED)
		{
			if ( HWMngr[i].rx_buf != NULL )
			{
				if ( HWMngr[i].rx_buf_index != 0 )
				{
					if (( HWMngr[i].flags & HWMAN_TIMEOUT_ENABLED) == HWMAN_TIMEOUT_ENABLED)
					{
						if ( HWMngr[i].timeout )
						{
							HWMngr[i].timeout --;
							if ( HWMngr[i].timeout == 0 )
							{
								HWMngr[i].timeout = HWMngr[i].timeout_reload_value;
								HWMngr[i].rxlen = HWMngr[i].rx_buf_index;
								HWMngr[i].rx_buf_index = 0;
								activate_process(HWMngr[i].process,1<<i,WAKEUP_FLAGS_UART_TO | WAKEUP_FLAGS_UART_RX);
								if (( HWMngr[i].status & HWMAN_SENTINEL_FOUND) == HWMAN_SENTINEL_FOUND)
									HWMngr[i].status &= ~HWMAN_SENTINEL_FOUND;

							}
						}
					}
				}
			}
		}
	}
}
