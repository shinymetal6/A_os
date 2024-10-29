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
 * onchip_uart.c
 *
 *  Created on: Oct 28, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#include "onchip_uart.h"
#include <string.h>

extern		UARTS_DriverStruct_t	*UARTS_DriverStruct[MAX_UARTS_DRIVERS];

static uint32_t onchip_uart_start(uint8_t handle)
{
OnChip_UART_Drv_TypeDef	*uarts_Drv;
	uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[handle]->uart_driver_private_data;
	HAL_UART_Receive_IT(uarts_Drv->uart, &uarts_Drv->rx_char, 1);

	return 0;
}

static uint32_t onchip_uart_stop(uint8_t handle)
{
	return 0;
}

static uint32_t onchip_uart_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t onchip_uart_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t onchip_uart_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t onchip_uart_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	UARTS_DriverStruct_t	OnChip_UART_Drv;

uint32_t onchip_uart_deinit(uint8_t handle)
{
	return uart_driver_unregister(&OnChip_UART_Drv);
}

static uint32_t onchip_uart_init(uint8_t handle)
{
OnChip_UART_Drv_TypeDef	*uarts_Drv;
	uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[handle]->uart_driver_private_data;
	uarts_Drv->timeout_reload_value = uarts_Drv->timeout;
	return 0;
}

static uint32_t	onchip_uart_send_buffer(uint8_t handle, uint8_t *buffer,uint8_t len)
{
OnChip_UART_Drv_TypeDef	*uarts_Drv = (OnChip_UART_Drv_TypeDef	*)UARTS_DriverStruct[handle]->uart_driver_private_data;
	return  HAL_UART_Transmit_IT(uarts_Drv->uart , buffer, len);
}

static uint32_t	onchip_uart_send_buffer_dma(uint8_t handle, uint8_t *buffer,uint8_t len)
{
OnChip_UART_Drv_TypeDef	*uarts_Drv = (OnChip_UART_Drv_TypeDef	*)UARTS_DriverStruct[handle]->uart_driver_private_data;
	return HAL_UART_Transmit_DMA(uarts_Drv->uart , buffer, len);
}

static uint32_t	onchip_uart_receive_buffer(uint8_t handle, uint8_t *buffer,uint8_t rx_buf_max_len)
{
OnChip_UART_Drv_TypeDef	*uarts_Drv = (OnChip_UART_Drv_TypeDef	*)UARTS_DriverStruct[handle]->uart_driver_private_data;
	uarts_Drv->data = buffer;
	return HAL_UART_Receive_IT(uarts_Drv->uart, &uarts_Drv->rx_char, 1);
}

static uint32_t	onchip_uart_receive_buffer_sentinel(uint8_t handle, uint8_t *buffer,uint8_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end)
{
OnChip_UART_Drv_TypeDef	*uarts_Drv = (OnChip_UART_Drv_TypeDef	*)UARTS_DriverStruct[handle]->uart_driver_private_data;
	uarts_Drv->sentinel_start = sentinel_start;
	uarts_Drv->sentinel_end   = sentinel_end;
	uarts_Drv->data = buffer;
	return HAL_UART_Receive_IT(uarts_Drv->uart, &uarts_Drv->rx_char, 1);
}

static uint32_t	onchip_uart_get_received_number_of_chars(uint8_t handle)
{
OnChip_UART_Drv_TypeDef	*uarts_Drv;
	uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[handle]->uart_driver_private_data;
	return (uint32_t )uarts_Drv->rx_num_chars;
}

UARTS_DriverStruct_t	OnChip_UART_Drv =
{
	.init = onchip_uart_init,
	.deinit = onchip_uart_deinit,
	.start = onchip_uart_start,
	.stop = onchip_uart_stop,
	.extended_action = onchip_uart_extended_actions,
	.get_status = onchip_uart_get_status,
	.get_values = onchip_uart_get_values,
	.get_received_number_of_chars = onchip_uart_get_received_number_of_chars,
	.set_values = onchip_uart_set_values,
	.send_buffer = onchip_uart_send_buffer,
	.send_buffer_dma = onchip_uart_send_buffer_dma,
	.receive_buffer = onchip_uart_receive_buffer,
	.receive_buffer_sentinel = onchip_uart_receive_buffer_sentinel,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.uart_driver_name = "onchip_uart",
};

uint32_t onchip_uart_allocate_driver(UARTS_DriverStruct_t *new_struct)
{
	memcpy(new_struct,&OnChip_UART_Drv,sizeof(OnChip_UART_Drv));
	return 0;
}

static uint8_t find_handle_from_uart(UART_HandleTypeDef *huart)
{
uint8_t	i;
OnChip_UART_Drv_TypeDef	*uarts_Drv;

	for(i=0;i<MAX_UARTS_DRIVERS;i++)
	{
		uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[i]->uart_driver_private_data;
		if ( huart == uarts_Drv->uart)
			return i;
	}
	return 255;
}

/***********************************/
/****	Interrupt functions 	****/
/***********************************/

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
uint8_t	handle;
OnChip_UART_Drv_TypeDef	*uarts_Drv;

	__disable_irq();
	if ( (handle = find_handle_from_uart(huart)) != 255)
	{
		uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[handle]->uart_driver_private_data;
		if (( uarts_Drv->flags & UART_WAKEUP_ON_TX) == UART_WAKEUP_ON_TX)
			activate_process(UARTS_DriverStruct[handle]->process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_TX);
	}
	__enable_irq();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
uint8_t	handle;
OnChip_UART_Drv_TypeDef	*uarts_Drv;
	__disable_irq();
	if ( (handle = find_handle_from_uart(huart)) != 255)
	{
		uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[handle]->uart_driver_private_data;
		if (uarts_Drv->data != NULL )
		{
			if ((uarts_Drv->sentinel_start == 0) && ( uarts_Drv->sentinel_end == 0 ))
			{
				uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
				uarts_Drv->timeout = uarts_Drv->timeout_reload_value;
				if (( uarts_Drv->flags & UART_WAKEUP_ON_RXCHAR) == UART_WAKEUP_ON_RXCHAR)
					activate_process(UARTS_DriverStruct[handle]->process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
				uarts_Drv->rx_index ++;
				if ( uarts_Drv->rx_index > uarts_Drv->rx_max_len )
				{
					uarts_Drv->rx_index = 0;
					if (( uarts_Drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
						activate_process(UARTS_DriverStruct[handle]->process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
				}
			}
			else
			{
				uarts_Drv->timeout = uarts_Drv->timeout_reload_value;
				if (( uarts_Drv->rx_char == uarts_Drv->sentinel_start) && ((uarts_Drv->flags & UART_SENTINEL_START_FOUND) != UART_SENTINEL_START_FOUND))
				{
					uarts_Drv->flags |= UART_SENTINEL_START_FOUND;
					uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
					uarts_Drv->rx_index = 1;
				}
				if (( uarts_Drv->rx_char == uarts_Drv->sentinel_end) && ((uarts_Drv->flags & UART_SENTINEL_START_FOUND) == UART_SENTINEL_START_FOUND))
				{
					uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
					uarts_Drv->flags |= UART_SENTINEL_END_FOUND;
					if (( uarts_Drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
					{
						uarts_Drv->rx_num_chars = uarts_Drv->rx_index;
						uarts_Drv->rx_index = 0;
						uarts_Drv->flags &= ~(UART_SENTINEL_START_FOUND | UART_SENTINEL_END_FOUND);
						activate_process(UARTS_DriverStruct[handle]->process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
					}
				}
				else
				{
					uarts_Drv->data[uarts_Drv->rx_index] = uarts_Drv->rx_char;
					uarts_Drv->rx_index ++;
					if ( uarts_Drv->rx_index > uarts_Drv->rx_max_len )
					{
						uarts_Drv->rx_index = 0;
						uarts_Drv->flags &= ~(UART_SENTINEL_START_FOUND | UART_SENTINEL_END_FOUND);
					}
				}
			}
		}
		HAL_UART_Receive_IT(uarts_Drv->uart, &uarts_Drv->rx_char, 1);
	}
	__enable_irq();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
uint8_t	handle;
OnChip_UART_Drv_TypeDef	*uarts_Drv;
	__disable_irq();
	if ( (handle = find_handle_from_uart(huart)) != 255)
	{
		uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[handle]->uart_driver_private_data;
		uarts_Drv->uart_error++;
		if (( uarts_Drv->flags & UART_WAKEUP_ON_ERRORS) == UART_WAKEUP_ON_ERRORS)
			activate_process(UARTS_DriverStruct[handle]->process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_ERR);
	}
}

void HAL_UART_RxTimeoutCheckCallback(void)
{
uint8_t	i;
OnChip_UART_Drv_TypeDef	*uarts_Drv;

	for(i=0;i<MAX_UARTS_DRIVERS;i++)
	{
		if ( UARTS_DriverStruct[i] != NULL )
		{
			uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[i]->uart_driver_private_data;
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
							activate_process(UARTS_DriverStruct[i]->process,uarts_Drv->wakeup_id,WAKEUP_FLAGS_UART_TO | WAKEUP_FLAGS_UART_RX);
					}
				}
			}
		}
	}
}


