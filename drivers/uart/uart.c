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
 * uart.c
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../kernel/kernel_opt.h"

#include "uart.h"
#include <string.h>

extern		UARTS_DriverStruct_t	UARTS_DriverStruct[MAX_UARTS_DRIVERS];
SYSTEM_RAM	uint8_t					last_uart_used_handle=0,uart_driver_request = 0;

ITCM_AREA_CODE  uint32_t uart_init(uint8_t handle)
{
UART_Drv_TypeDef	*uarts_Drv;
	uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[handle].uart_driver_private_data;
	uarts_Drv->timeout_reload_value = uarts_Drv->timeout;
	return 0;
}

ITCM_AREA_CODE  uint32_t uart_start(uint8_t handle)
{
UART_Drv_TypeDef	*uarts_Drv;
	uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[handle].uart_driver_private_data;
	HAL_UART_Receive_IT(uarts_Drv->uart, &uarts_Drv->rx_char, 1);

	return 0;
}

ITCM_AREA_CODE  uint32_t uart_get_status(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE  uint32_t	uart_send(uint8_t handle, uint8_t *buffer,uint8_t len)
{
UART_Drv_TypeDef	*uarts_Drv = (UART_Drv_TypeDef	*)UARTS_DriverStruct[handle].uart_driver_private_data;
	if ( (uarts_Drv->config & UART_USE_DMA_TX) == UART_USE_DMA_TX )
		return HAL_UART_Transmit_DMA(uarts_Drv->uart , buffer, len);
	else
		return  HAL_UART_Transmit_IT(uarts_Drv->uart , buffer, len);
}

ITCM_AREA_CODE  uint32_t	uart_receive_buffer(uint8_t handle, uint8_t *buffer,uint8_t rx_buf_max_len)
{
UART_Drv_TypeDef	*uarts_Drv = (UART_Drv_TypeDef	*)UARTS_DriverStruct[handle].uart_driver_private_data;
	uarts_Drv->data = buffer;
	return HAL_UART_Receive_IT(uarts_Drv->uart, &uarts_Drv->rx_char, 1);
}

ITCM_AREA_CODE  uint32_t	uart_receive_buffer_sentinel(uint8_t handle, uint8_t *buffer,uint8_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end)
{
UART_Drv_TypeDef	*uarts_Drv = (UART_Drv_TypeDef	*)UARTS_DriverStruct[handle].uart_driver_private_data;
	uarts_Drv->sentinel_start = sentinel_start;
	uarts_Drv->sentinel_end   = sentinel_end;
	uarts_Drv->data = buffer;
	return HAL_UART_Receive_IT(uarts_Drv->uart, &uarts_Drv->rx_char, 1);
}

ITCM_AREA_CODE  uint32_t	uart_get_rxlen(uint8_t handle)
{
UART_Drv_TypeDef	*uarts_Drv;
	uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[handle].uart_driver_private_data;
	return (uint32_t )uarts_Drv->rx_num_chars;
}

ITCM_AREA_CODE uint32_t	uart_register(UART_Drv_TypeDef *uart_driver_private_data,uint32_t driver_flags,uint32_t uart_flags)
{
UART_Drv_TypeDef	*uarts_Drv;
	if ( UARTS_DriverStruct[last_uart_used_handle].process == 0 )
	{
		UARTS_DriverStruct[last_uart_used_handle].process = get_current_process();
		UARTS_DriverStruct[last_uart_used_handle].flags |= driver_flags;
		UARTS_DriverStruct[last_uart_used_handle].uart_driver_private_data = uart_driver_private_data;

		uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[last_uart_used_handle].uart_driver_private_data;
		uarts_Drv->flags |= uart_flags;
		uarts_Drv->timeout_reload_value = uarts_Drv->timeout;
		UARTS_DriverStruct[last_uart_used_handle].status = DRIVER_STATUS_REQUESTED;

		last_uart_used_handle++;
		uart_driver_request++;
		return last_uart_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

/* Interrupt area */
ITCM_AREA_CODE static uint8_t find_handle_from_uart(UART_HandleTypeDef *huart)
{
uint8_t	i;
UART_Drv_TypeDef	*uarts_Drv;

	for(i=0;i<MAX_UARTS_DRIVERS;i++)
	{
		uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[i].uart_driver_private_data;
		if ( huart == uarts_Drv->uart)
			return i;
	}
	return 255;
}

/***********************************/
/****	Interrupt functions 	****/
/***********************************/

ITCM_AREA_CODE void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
uint8_t	handle;
UART_Drv_TypeDef	*uarts_Drv;

	__disable_irq();
	if ( (handle = find_handle_from_uart(huart)) != 255)
	{
		uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[handle].uart_driver_private_data;
		if (( uarts_Drv->flags & UART_WAKEUP_ON_TX) == UART_WAKEUP_ON_TX)
			activate_process(UARTS_DriverStruct[handle].process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_TX);
	}
	__enable_irq();
}

ITCM_AREA_CODE void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
uint8_t	handle;
UART_Drv_TypeDef	*uarts_Drv;
	__disable_irq();
	if ( (handle = find_handle_from_uart(huart)) != 255)
	{
		uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[handle].uart_driver_private_data;
		if (uarts_Drv->data != NULL )
		{
			if ((uarts_Drv->sentinel_start == 0) && ( uarts_Drv->sentinel_end == 0 ))
			{
				uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
				uarts_Drv->timeout = uarts_Drv->timeout_reload_value;
				uarts_Drv->rx_index ++;
				if ( uarts_Drv->rx_index >= uarts_Drv->rx_max_len )
				{
					uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
					uarts_Drv->rx_index = 0;
					if (( uarts_Drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
						activate_process(UARTS_DriverStruct[handle].process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
				}
				if (( uarts_Drv->flags & UART_WAKEUP_ON_RXCHAR) == UART_WAKEUP_ON_RXCHAR)
					activate_process(UARTS_DriverStruct[handle].process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
			}
			else if ((uarts_Drv->sentinel_start != 0) && ( uarts_Drv->sentinel_end == 0 ))
			{
				uarts_Drv->timeout = uarts_Drv->timeout_reload_value;
				if ((uarts_Drv->flags & UART_SENTINEL_START_FOUND) == UART_SENTINEL_START_FOUND)
				{
					uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
					uarts_Drv->rx_index ++;
					if ( uarts_Drv->rx_index > uarts_Drv->rx_max_len )
					{
						uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
						uarts_Drv->rx_index = 0;
						uarts_Drv->flags &= ~UART_SENTINEL_START_FOUND;
						if (( uarts_Drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
							activate_process(UARTS_DriverStruct[handle].process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
					}
					if (( uarts_Drv->flags & UART_WAKEUP_ON_RXCHAR) == UART_WAKEUP_ON_RXCHAR)
						activate_process(UARTS_DriverStruct[handle].process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
				}
				else
				{
					if ( uarts_Drv->rx_char == uarts_Drv->sentinel_start)
					{
						if ( uarts_Drv->rx_char == uarts_Drv->sentinel_start)
						{
							uarts_Drv->flags |= UART_SENTINEL_START_FOUND;
							uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
							uarts_Drv->rx_index = 1;
							uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
						}
					}
				}
			}
			else if ((uarts_Drv->sentinel_start != 0) && ( uarts_Drv->sentinel_end != 0 ))
			{
				uarts_Drv->timeout = uarts_Drv->timeout_reload_value;
				if (( uarts_Drv->rx_char == uarts_Drv->sentinel_start) && ((uarts_Drv->flags & UART_SENTINEL_START_FOUND) != UART_SENTINEL_START_FOUND))
				{
					uarts_Drv->flags |= UART_SENTINEL_START_FOUND;
					uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
					uarts_Drv->rx_index = 1;
					uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
				}
				if (( uarts_Drv->rx_char == uarts_Drv->sentinel_end) && ((uarts_Drv->flags & UART_SENTINEL_START_FOUND) == UART_SENTINEL_START_FOUND))
				{
					uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
					uarts_Drv->flags |= UART_SENTINEL_END_FOUND;
					if (( uarts_Drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
					{
						uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
						uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
						uarts_Drv->rx_index = 0;
						uarts_Drv->flags &= ~(UART_SENTINEL_START_FOUND | UART_SENTINEL_END_FOUND);
						activate_process(UARTS_DriverStruct[handle].process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
					}
				}
				else
				{
					uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
					uarts_Drv->rx_index ++;
					uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
					if ( uarts_Drv->rx_index > uarts_Drv->rx_max_len )
					{
						uarts_Drv->rx_index = 0;
						uarts_Drv->flags &= ~(UART_SENTINEL_START_FOUND | UART_SENTINEL_END_FOUND);
					}
				}
			}
		}
		uarts_Drv->timeout = uarts_Drv->timeout_reload_value;
		HAL_UART_Receive_IT(uarts_Drv->uart, &uarts_Drv->rx_char, 1);
	}
	__enable_irq();
}

ITCM_AREA_CODE void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
uint8_t	handle;
UART_Drv_TypeDef	*uarts_Drv;
	__disable_irq();
	if ( (handle = find_handle_from_uart(huart)) != 255)
	{
		uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[handle].uart_driver_private_data;
		uarts_Drv->uart_error++;
		if (( uarts_Drv->flags & UART_WAKEUP_ON_ERRORS) == UART_WAKEUP_ON_ERRORS)
			activate_process(UARTS_DriverStruct[handle].process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_ERR);
		uarts_Drv->timeout = uarts_Drv->timeout_reload_value;
	}
}

ITCM_AREA_CODE void HAL_UART_RxTimeoutCheckCallback(void)
{
uint8_t	i;
UART_Drv_TypeDef	*uarts_Drv;

	for(i=0;i<MAX_UARTS_DRIVERS;i++)
	{
		if ( UARTS_DriverStruct[i].process != 0 )
		{
			uarts_Drv = (UART_Drv_TypeDef *)UARTS_DriverStruct[i].uart_driver_private_data;
			if ( uarts_Drv->timeout )
			{
				if ( uarts_Drv->rx_index )
				{
					uarts_Drv->timeout--;
					if ( uarts_Drv->timeout == 0 )
					{
						uarts_Drv->timeout = uarts_Drv->timeout_reload_value;
						uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
						uarts_Drv->rx_index = 0;
						if (( uarts_Drv->flags & UART_WAKEUP_ON_TIMEOUT) == UART_WAKEUP_ON_TIMEOUT)
							activate_process(UARTS_DriverStruct[i].process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_TO | WAKEUP_FLAGS_UART_RX);
					}
				}
			}
		}
	}
}




