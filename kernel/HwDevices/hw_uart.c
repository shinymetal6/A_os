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
#include "../system_default.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "hw_uart.h"
//#include "../kernel_opt.h"

extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	Asys_t			Asys;

#ifdef A_HAS_UARTS

extern	HW_Uart_t		HW_Uart[A_MAX_UART];
#define	NO_SENTINEL	0

ITCM_AREA_CODE uint32_t hw_send_uart(uint8_t uart,uint8_t *ptr,uint16_t len)
{
	if ( HWMngr[uart].process != Asys.current_process )
		return HW_UART_ERROR;
	if ( HW_Uart[uart-HW_UART1].hwuart_handle == NULL )
		return HW_UART_ERROR;
	return  HAL_UART_Transmit_IT(HW_Uart[uart-HW_UART1].hwuart_handle, ptr, len);
}

ITCM_AREA_CODE uint32_t hw_send_uart_dma(uint8_t uart,uint8_t *ptr,uint16_t len)
{
	if ( HWMngr[uart].process != Asys.current_process )
		return HW_UART_ERROR;
	if ( HW_Uart[uart-HW_UART1].hwuart_handle == NULL )
		return HW_UART_ERROR;
	return HAL_UART_Transmit_DMA(HW_Uart[uart-HW_UART1].hwuart_handle, ptr, len);
}

ITCM_AREA_CODE uint32_t hw_receive_uart(uint8_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint16_t timeout)
{
	if ( HWMngr[uart].process == Asys.current_process )
	{
		HW_Uart[uart-HW_UART1].sentinel_start = HW_Uart[uart-HW_UART1].sentinel_end = NO_SENTINEL;
		HWMngr[uart].status &= ~HWMAN_SENTINEL_ENABLE;
		HWMngr[uart].rx_buf = rx_buf;
		HWMngr[uart].rx_buf_max_len = rx_buf_max_len;
		if ( timeout )
		{
			HW_Uart[uart-HW_UART1].timeout = HW_Uart[uart-HW_UART1].timeout_reload_value = timeout;
			HWMngr[uart].flags |= HWMAN_TIMEOUT_ENABLED;
		}
		HWMngr[uart].rx_buf_index = 0;
		return	HAL_UART_Receive_IT(HW_Uart[uart-HW_UART1].hwuart_handle, &HW_Uart[uart-HW_UART1].rx_char, 1);
	}
	return HW_UART_ERROR;
}

ITCM_AREA_CODE uint32_t hw_receive_uart_sentinel(uint8_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end,uint16_t timeout)
{
	if ( HW_Uart[uart-HW_UART1].hwuart_handle == NULL )
		return HW_UART_ERROR;
	if ( HWMngr[uart].process == Asys.current_process )
	{
		if ( HW_Uart[uart-HW_UART1].hwuart_handle == NULL )
			return HW_UART_ERROR;
		HW_Uart[uart-HW_UART1].sentinel_start = sentinel_start;
		HW_Uart[uart-HW_UART1].sentinel_end = sentinel_end;
		HWMngr[uart].status &= ~HWMAN_SENTINEL_FOUND;
		HWMngr[uart].status |= HWMAN_SENTINEL_ENABLE;

		HWMngr[uart].rx_buf = rx_buf;
		HWMngr[uart].rx_buf_max_len = rx_buf_max_len;
		if ( timeout )
		{
			HW_Uart[uart-HW_UART1].timeout = HW_Uart[uart-HW_UART1].timeout_reload_value = timeout;
			HWMngr[uart].flags |= HWMAN_TIMEOUT_ENABLED;
		}
		HWMngr[uart].rx_buf_index = 0;
		return	HAL_UART_Receive_IT(HW_Uart[uart-HW_UART1].hwuart_handle, &HW_Uart[uart-HW_UART1].rx_char, 1);
	}
	return HW_UART_ERROR;
}

ITCM_AREA_CODE uint32_t hw_receive_uart_sentinel_clear(uint8_t uart)
{
	if ( HWMngr[uart].process == Asys.current_process )
	{
		HW_Uart[uart-HW_UART1].sentinel_start = HW_Uart[uart-HW_UART1].sentinel_end = NO_SENTINEL;
		return HW_UART_OK;
	}
	return HW_UART_ERROR;
}

ITCM_AREA_CODE uint16_t hw_get_uart_receive_len(uint8_t uart)
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
	for(i=HW_UART1;i<HW_UART1+A_MAX_UART;i++)
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
	for(i=HW_UART1;i<HW_UART1+A_MAX_UART;i++)
	{
		if (HW_Uart[i-HW_UART1].hwuart_handle == huart)
		{
			if (( HWMngr[i].status & HWMAN_STATUS_ALLOCATED) == HWMAN_STATUS_ALLOCATED)
			{
				if ( HWMngr[i].rx_buf == NULL )
				{
					HAL_UART_Receive_IT(huart, &HW_Uart[i-HW_UART1].rx_char, 1);
					__enable_irq();
					return;
				}
				if (( HWMngr[i].status & HWMAN_SENTINEL_ENABLE) == HWMAN_SENTINEL_ENABLE)
				{
					if (( HWMngr[i].status & HWMAN_SENTINEL_FOUND) != HWMAN_SENTINEL_FOUND)
					{
						if ( HW_Uart[i-HW_UART1].rx_char == HW_Uart[i-HW_UART1].sentinel_start)
						{
							HWMngr[i].status |= HWMAN_SENTINEL_FOUND;
							HWMngr[i].rx_buf_index=0;
						}
						else
						{
							HAL_UART_Receive_IT(huart, &HW_Uart[i-HW_UART1].rx_char, 1);
							__enable_irq();
							return;
						}
					}

					if (( HWMngr[i].status & HWMAN_SENTINEL_FOUND) == HWMAN_SENTINEL_FOUND)
					{
						HWMngr[i].rx_buf[HWMngr[i].rx_buf_index] = HW_Uart[i-HW_UART1].rx_char;
						HW_Uart[i-HW_UART1].timeout = HW_Uart[i-HW_UART1].timeout_reload_value;
						HWMngr[i].rx_buf_index++;
						if ( HWMngr[i].rx_buf_index == HWMngr[i].rx_buf_max_len)
						{
							HWMngr[i].rxlen = HWMngr[i].rx_buf_index;
							HWMngr[i].rx_buf_index = 0;
							HWMngr[i].status &= ~HWMAN_SENTINEL_FOUND;
							if ( HWMngr[i].irq_callback != NULL )
								HWMngr[i].irq_callback();
							activate_process(HWMngr[i].process,1<<i,WAKEUP_FLAGS_UART_RX);
						}
						if (HW_Uart[i-HW_UART1].rx_char == HW_Uart[i-HW_UART1].sentinel_end )
						{
							HWMngr[i].rxlen = HWMngr[i].rx_buf_index;
							HWMngr[i].rx_buf_index = 0;
							HWMngr[i].status &= ~HWMAN_SENTINEL_FOUND;
							if ( HWMngr[i].irq_callback != NULL )
								HWMngr[i].irq_callback();
							activate_process(HWMngr[i].process,1<<i,WAKEUP_FLAGS_UART_RX);
						}
						HAL_UART_Receive_IT(huart, &HW_Uart[i-HW_UART1].rx_char, 1);
						__enable_irq();
						return;
					}
				}
				else
				{
					HWMngr[i].rx_buf[HWMngr[i].rx_buf_index] = HW_Uart[i-HW_UART1].rx_char;
					HWMngr[i].rx_buf_index++;
					if ( HWMngr[i].rx_buf_index == HWMngr[i].rx_buf_max_len)
					{
						HWMngr[i].rxlen = HWMngr[i].rx_buf_index;
						HWMngr[i].rx_buf_index = 0;
						if ( HWMngr[i].irq_callback != NULL )
							HWMngr[i].irq_callback();
						activate_process(HWMngr[i].process,1<<i,WAKEUP_FLAGS_UART_RX);
					}
					HAL_UART_Receive_IT(huart, &HW_Uart[i-HW_UART1].rx_char, 1);
					__enable_irq();
					return;
				}
			}
		}
	}
	__enable_irq();
}

void HAL_UART_RxTimeoutCheckCallback(void)
{
uint32_t	i;
	for(i=HW_UART1;i<HW_UART1+A_MAX_UART;i++)
	{
		if (( HWMngr[i].status & HWMAN_STATUS_ALLOCATED) == HWMAN_STATUS_ALLOCATED)
		{
			if ( HWMngr[i].rx_buf != NULL )
			{
				if ( HWMngr[i].rx_buf_index != 0 )
				{
					if (( HWMngr[i].flags & HWMAN_TIMEOUT_ENABLED) == HWMAN_TIMEOUT_ENABLED)
					{
						if ( HW_Uart[i-HW_UART1].timeout )
						{
							HW_Uart[i-HW_UART1].timeout --;
							if ( HW_Uart[i-HW_UART1].timeout == 0 )
							{
								HW_Uart[i-HW_UART1].timeout = HW_Uart[i-HW_UART1].timeout_reload_value;
								HWMngr[i].rxlen = HWMngr[i].rx_buf_index;
								HWMngr[i].rx_buf[HWMngr[i].rx_buf_index] = 0;
								HWMngr[i].rx_buf_index = 0;
								if ( HWMngr[i].irq_callback != NULL )
									HWMngr[i].irq_callback();
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
#endif

void A_hw_uart_init(void)
{
#ifdef	A_HAS_UART1
	HW_Uart[HW_UART1-HW_UART1].hwuart_handle = &huart1;
	HW_Uart[HW_UART1-HW_UART1].hwuart_index  = HW_UART1;
#endif
#ifdef	A_HAS_UART2
	HW_Uart[HW_UART2-HW_UART1].hwuart_handle = &huart2;
	HW_Uart[HW_UART2-HW_UART1].hwuart_index  = HW_UART2;
#endif
#ifdef	A_HAS_UART3
	HW_Uart[HW_UART3-HW_UART1].hwuart_handle = &huart3;
	HW_Uart[HW_UART3-HW_UART1].hwuart_index  = HW_UART3;
#endif
#ifdef	A_HAS_UART4
	HW_Uart[HW_UART4-HW_UART1].hwuart_handle = &huart4;
	HW_Uart[HW_UART4-HW_UART1].hwuart_index  = HW_UART4;
#endif
#ifdef	A_HAS_UART5
	HW_Uart[HW_UART5-HW_UART1].hwuart_handle = &huart5;
	HW_Uart[HW_UART5-HW_UART1].hwuart_index  = HW_UART5;
#endif
#ifdef	A_HAS_UART6
	HW_Uart[HW_UART6-HW_UART1].hwuart_handle = &huart6;
	HW_Uart[HW_UART6-HW_UART1].hwuart_index  = HW_UART6;
#endif
#ifdef	A_HAS_UART7
	HW_Uart[HW_UART7-HW_UART1].hwuart_handle = &huart7;
	HW_Uart[HW_UART7-HW_UART1].hwuart_index  = HW_UART7;
#endif
#ifdef	A_HAS_UART8
	HW_Uart[HW_UART8-HW_UART1].hwuart_handle = &huart8;
	HW_Uart[HW_UART8-HW_UART1].hwuart_index  = HW_UART8;
#endif
}
