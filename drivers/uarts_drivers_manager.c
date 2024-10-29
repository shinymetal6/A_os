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
 * uarts_drivers_manager.c
 *
 *  Created on: Oct 28, 2024
 *      Author: fil
 */

#include "main.h"
#include "../kernel/system_default.h"
#include "../kernel/A.h"
#include "../kernel/A_exported_functions.h"
#include "../kernel/scheduler.h"
#include <string.h>
#include "uarts_drivers_manager.h"

extern		UARTS_DriverStruct_t	*UARTS_DriverStruct[MAX_UARTS_DRIVERS];
SYSTEM_RAM	uint8_t					last_uart_used_handle=0,uart_driver_request = 0;

uint32_t	uart_driver_register(UARTS_DriverStruct_t *uart_driver,uint32_t *uart_driver_private_data,uint32_t driver_flags,uint32_t uart_flags)
{
OnChip_UART_Drv_TypeDef	*uarts_Drv;
	if ( UARTS_DriverStruct[last_uart_used_handle] == NULL )
	{
		UARTS_DriverStruct[last_uart_used_handle] = uart_driver;
		UARTS_DriverStruct[last_uart_used_handle]->process = get_current_process();
		UARTS_DriverStruct[last_uart_used_handle]->flags |= driver_flags;
		UARTS_DriverStruct[last_uart_used_handle]->uart_driver_private_data = uart_driver_private_data;

		uarts_Drv = (OnChip_UART_Drv_TypeDef *)UARTS_DriverStruct[last_uart_used_handle]->uart_driver_private_data;
		uarts_Drv->flags |= uart_flags;
		if (uart_driver->periodic_before_check_timers_callback != NULL  )
			set_before_check_timers_callback(uart_driver->periodic_before_check_timers_callback);
		if (uart_driver->periodic_after_check_timers_callback != NULL  )
			set_after_check_timers_callback(uart_driver->periodic_after_check_timers_callback);
		UARTS_DriverStruct[last_uart_used_handle]->status = DRIVER_STATUS_REQUESTED;

		last_uart_used_handle++;
		uart_driver_request++;
		return last_uart_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

uint32_t	uart_driver_unregister(UARTS_DriverStruct_t *uart_driver)
{
uint32_t	i;
	for(i=0;i<MAX_DRIVERS;i++)
	{
		if ( strcmp(UARTS_DriverStruct[i]->uart_driver_name,uart_driver->uart_driver_name) == 0 )
		{
			UARTS_DriverStruct[i] = NULL;
			last_uart_used_handle--;
			return 0;
		}
	}
	return 1;
}

uint32_t uart_driver_scan(void)
{
uint32_t	i,drv_ret;
	if (uart_driver_request )
	{
		for(i=0;i<MAX_DRIVERS;i++)
		{
			if ( UARTS_DriverStruct[i] == NULL )
				return DRIVER_STATUS_INITPEND;
			if (( UARTS_DriverStruct[i]->status & DRIVER_STATUS_REQUESTED) ==  DRIVER_STATUS_REQUESTED)
			{
				UARTS_DriverStruct[i]->status = DRIVER_STATUS_IN_USE;
				if ( UARTS_DriverStruct[i]->init != NULL )
				{
					if ( (drv_ret = UARTS_DriverStruct[i]->init(i)) == DRIVER_REQUEST_FAILED)
						UARTS_DriverStruct[i]->status = DRIVER_STATUS_FAILED;
					else
					{
						if (( UARTS_DriverStruct[i]->flags & DRIVER_FLAGS_AUTOSTART) == DRIVER_FLAGS_AUTOSTART)
						{
							if ( UARTS_DriverStruct[i]->start != NULL )
								UARTS_DriverStruct[i]->start(i);
						}

					}
				}
				uart_driver_request --;
				if ( uart_driver_request == 0)
					return drv_ret;
			}
		}
	}
	return DRIVER_STATUS_INITIALIZED;
}

uint32_t uart_driver_start(uint32_t handle)
{
	if ( UARTS_DriverStruct[handle]->start != NULL )
		return UARTS_DriverStruct[handle]->start(handle);
	return DRIVER_REQUEST_FAILED;
}

uint32_t uart_driver_get_values(uint32_t handle,uint8_t *values,uint8_t values_number)
{
	if ( UARTS_DriverStruct[handle]->get_values != NULL )
		return UARTS_DriverStruct[handle]->get_values(handle,values,values_number);
	return DRIVER_REQUEST_FAILED;
}

uint32_t uart_driver_set_values(uint32_t handle,uint8_t *values,uint8_t values_number)
{
	if ( UARTS_DriverStruct[handle]->set_values != NULL )
		return UARTS_DriverStruct[handle]->set_values(handle,values,values_number);
	return DRIVER_REQUEST_FAILED;
}

uint32_t uart_driver_extended_action(uint32_t handle,uint32_t *action)
{
	if ( UARTS_DriverStruct[handle]->extended_action != NULL )
		return UARTS_DriverStruct[handle]->extended_action(handle,action);
	return DRIVER_REQUEST_FAILED;
}

uint32_t uart_driver_send_buffer(uint8_t handle, uint8_t *buffer,uint16_t len)
{
	if ( UARTS_DriverStruct[handle]->send_buffer != NULL )
		return UARTS_DriverStruct[handle]->send_buffer(handle,buffer,len);
	return DRIVER_REQUEST_FAILED;
}

uint32_t uart_driver_send_buffer_dma(uint8_t handle, uint8_t *buffer,uint16_t len)
{
	if ( UARTS_DriverStruct[handle]->send_buffer_dma != NULL )
		return UARTS_DriverStruct[handle]->send_buffer_dma(handle,buffer,len);
	return DRIVER_REQUEST_FAILED;
}

uint32_t uart_driver_receive_buffer(uint8_t handle, uint8_t *buffer,uint16_t rx_buf_max_len)
{
	if ( UARTS_DriverStruct[handle]->receive_buffer != NULL )
		return UARTS_DriverStruct[handle]->receive_buffer(handle,buffer,rx_buf_max_len);
	return DRIVER_REQUEST_FAILED;
}

uint32_t uart_driver_receive_buffer_sentinel(uint8_t handle, uint8_t *buffer,uint16_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end)
{
	if ( UARTS_DriverStruct[handle]->receive_buffer_sentinel != NULL )
		return UARTS_DriverStruct[handle]->receive_buffer_sentinel(handle,buffer,rx_buf_max_len,sentinel_start,sentinel_end);
	return DRIVER_REQUEST_FAILED;
}

uint32_t uart_driver_get_received_number_of_chars(uint8_t handle)
{
	if ( UARTS_DriverStruct[handle]->receive_buffer != NULL )
		return UARTS_DriverStruct[handle]->get_received_number_of_chars(handle);
	return DRIVER_REQUEST_FAILED;
}

uint32_t 	uart_driver_init(void)
{
uint32_t	i,drv_ret=0;
uint32_t size;

	size = sizeof(UARTS_DriverStruct_t) / 4;

	for(i=0;i<MAX_UARTS_DRIVERS;i++)
	{
		A_clear32((uint32_t	*)&UARTS_DriverStruct[i],size);
	}
	set_before_check_timers_callback(HAL_UART_RxTimeoutCheckCallback);
	return drv_ret;
}


