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

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#ifdef A_OS_UART_ENABLED

#include "uart.h"
#include <string.h>

UART_DriverStruct_t	*uart_drv_ptr;

ITCM_AREA_CODE  uint32_t uart_init(UART_DriverStruct_t *uart_drv)
{
	uart_drv->timeout_reload_value = uart_drv->timeout;
	return 0;
}

ITCM_AREA_CODE  uint32_t uart_get_status(UART_DriverStruct_t *uart_drv)
{
	return 0;
}

uint32_t ret_val = 0;
ITCM_AREA_CODE  uint32_t	uart_send(UART_DriverStruct_t *uart_drv, uint8_t *buffer,uint16_t len)
{
	if ( (uart_drv->flags & UART_USES_DMA_TX) == UART_USES_DMA_TX )
		ret_val = HAL_UART_Transmit_DMA(uart_drv->uart , buffer, len);
	else
		ret_val = HAL_UART_Transmit_IT(uart_drv->uart , buffer, len);
	return ret_val;
}

ITCM_AREA_CODE  uint32_t	uart_start_receive(UART_DriverStruct_t *uart_drv)
{
uint32_t ret_val;
	if ( (uart_drv->flags & UART_USES_DMA_RX) == UART_USES_DMA_RX )
	{
		ret_val = HAL_UARTEx_ReceiveToIdle_DMA(uart_drv->uart, uart_drv->data, uart_drv->rx_max_len);
	    __HAL_DMA_DISABLE_IT(uart_drv->uart->hdmarx, DMA_IT_HT);
	    return ret_val;
	}
	else
		return HAL_UART_Receive_IT(uart_drv->uart, &uart_drv->rx_char, 1);
}

ITCM_AREA_CODE  uint32_t	uart_get_rxlen(UART_DriverStruct_t *uart_drv)
{
	return (uint32_t )uart_drv->rx_num_chars;
}

ITCM_AREA_CODE  uint32_t	uart_set_rxlen(UART_DriverStruct_t *uart_drv,uint16_t rx_max_len)
{
	uart_drv->rx_max_len = rx_max_len;
	return 0;
}


extern void UART_Driver_RxTimeoutCheckCallback(uint32_t *param);

ITCM_AREA_CODE uint32_t	uart_register(UART_DriverStruct_t *uart_drv)
{
UART_DriverStruct_t *eptr;

	if ( uart_drv->uart == NULL)
		return DRIVER_REQUEST_FAILED;

	if ( uart_drv_ptr == NULL)
	{
		uart_drv_ptr = uart_drv;
		uart_drv->next_uart = NULL;
	}
	else
	{
		eptr = uart_drv_ptr;
		while(eptr->next_uart != NULL)
			eptr = (UART_DriverStruct_t *)eptr->next_uart;
		eptr->next_uart = (uint32_t *)uart_drv;
		uart_drv->next_uart = NULL;
	}
	uart_drv->process = get_current_process();
	uart_drv->timeout_reload_value = uart_drv->timeout;
	if ( uart_drv->uart->hdmarx == NULL )
		uart_drv->flags &= ~UART_USES_DMA_RX;
	if ( uart_drv->uart->hdmatx == NULL )
		uart_drv->flags &= ~UART_USES_DMA_TX;
	if ( (uart_drv->flags & UART_USES_DMA_RX) == 0 )
		set_before_check_timers_callback(UART_Driver_RxTimeoutCheckCallback,(uint32_t *)uart_drv);

	return 0;
}

/***********************************/
/****	Interrupt functions 	****/
/***********************************/
ITCM_AREA_CODE UART_DriverStruct_t *find_uart(UART_HandleTypeDef *huart)
{
UART_DriverStruct_t *eptr;

	eptr = uart_drv_ptr;
	while(eptr != NULL)
	{
		if ( eptr->uart == huart )
			return eptr;
		if ( eptr->next_uart == NULL )
			return NULL;
		eptr = (UART_DriverStruct_t *)eptr->next_uart;
	}
	return NULL;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
UART_DriverStruct_t	*uart_drv;
	__disable_irq();
	if ( (uart_drv = find_uart(huart)) != NULL)
	{
		uart_drv->uart_error++;
		if (( uart_drv->flags & UART_WAKEUP_ON_ERRORS) == UART_WAKEUP_ON_ERRORS)
			activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_ERR);
		uart_drv->timeout = uart_drv->timeout_reload_value;
		uart_drv->rx_errors ++;
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
UART_DriverStruct_t	*uart_drv;

	__disable_irq();
	if ( (uart_drv = find_uart(huart)) != NULL)
	{
		if (( uart_drv->flags & UART_WAKEUP_ON_TX) == UART_WAKEUP_ON_TX)
			activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_TX);
	}
	__enable_irq();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
UART_DriverStruct_t	*uart_drv;
	__disable_irq();
	if ( (uart_drv = find_uart(huart)) != NULL)
	{
		if (uart_drv->data != NULL )
		{
				uart_drv->data[uart_drv->rx_index] = uart_drv->rx_char;
				uart_drv->timeout = uart_drv->timeout_reload_value;
				uart_drv->rx_index ++;
				if ( uart_drv->rx_index >= uart_drv->rx_max_len )
				{
					uart_drv->rx_num_chars = uart_drv->rx_index;
					uart_drv->rx_index = 0;
					if (( uart_drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
						activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
				}
				if (( uart_drv->flags & UART_WAKEUP_ON_RXCHAR) == UART_WAKEUP_ON_RXCHAR)
					activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
		}
		uart_drv->timeout = uart_drv->timeout_reload_value;
		HAL_UART_Receive_IT(uart_drv->uart, &uart_drv->rx_char, 1);
	}
	__enable_irq();
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
UART_DriverStruct_t	*uart_drv;
	__disable_irq();
	if ( (uart_drv = find_uart(huart)) != NULL)
	{
		if (uart_drv->data != NULL )
		{
			uart_drv->rx_num_chars = uart_drv->rx_max_len - __HAL_DMA_GET_COUNTER(uart_drv->uart->hdmarx);
			if (( uart_drv->flags & UART_WAKEUP_ON_RXFULL) == UART_WAKEUP_ON_RXFULL)
				activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_RX);
			uart_start_receive(uart_drv);
		}
	}
	__enable_irq();
}

ITCM_AREA_CODE void UART_Driver_RxTimeoutCheckCallback(uint32_t *param)
{
UART_DriverStruct_t	*uart_drv = (UART_DriverStruct_t *)param;
	if ( uart_drv->process != 0 )
	{
		if (( uart_drv->rx_index ) && ( uart_drv->rx_num_chars != uart_drv->rx_max_len))
		{
			if ( uart_drv->timeout )
				uart_drv->timeout--;
			if ( uart_drv->timeout == 0 )
			{
				uart_drv->rx_num_chars = uart_drv->rx_index;
				uart_drv->rx_index = 0;
				uart_drv->timeout = uart_drv->timeout_reload_value;
				if (( uart_drv->flags & UART_WAKEUP_ON_TIMEOUT) == UART_WAKEUP_ON_TIMEOUT)
					activate_process(uart_drv->process,uart_drv->wakeup_id,WAKEUP_FLAGS_UART_TO | WAKEUP_FLAGS_UART_RX);
			}
		}
	}
}


#endif // #ifdef A_OS_UART_ENABLED


