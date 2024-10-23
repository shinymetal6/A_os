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
 * drivers_manager.c
 *
 *  Created on: Oct 15, 2024
 *      Author: fil
 */

#include "main.h"
#include "../kernel/system_default.h"
#include "../kernel/A.h"
#include "../kernel/A_exported_functions.h"
#include "../kernel/scheduler.h"
#include <string.h>
#include "drivers_manager.h"

extern		DriverStruct_t	*DriverStruct[MAX_DRIVERS];
SYSTEM_RAM	uint8_t			last_used_handle=0,driver_request = 0;

uint32_t	driver_register(DriverStruct_t *driver,uint32_t *private_drv_struct,uint32_t flags)
{
	if ( DriverStruct[last_used_handle] == NULL )
	{
		DriverStruct[last_used_handle] = driver;
		DriverStruct[last_used_handle]->process = get_current_process();
		DriverStruct[last_used_handle]->flags |= flags;
		DriverStruct[last_used_handle]->driver_private_data = private_drv_struct;

		if (driver->periodic_before_check_timers_callback != NULL  )
			set_before_check_timers_callback(driver->periodic_before_check_timers_callback);
		if (driver->periodic_after_check_timers_callback != NULL  )
			set_after_check_timers_callback(driver->periodic_after_check_timers_callback);
		DriverStruct[last_used_handle]->status = DRIVER_STATUS_REQUESTED;

		last_used_handle++;
		driver_request++;
		return last_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

uint32_t	driver_unregister(DriverStruct_t *driver)
{
uint32_t	i;
	for(i=0;i<MAX_DRIVERS;i++)
	{
		if ( strcmp(DriverStruct[i]->driver_name,driver->driver_name) == 0 )
		{
			DriverStruct[i] = NULL;
			last_used_handle--;
			return 0;
		}
	}
	return 1;
}

uint32_t driver_scan(void)
{
uint32_t	i,drv_ret;
	if (driver_request )
	{
		for(i=0;i<MAX_DRIVERS;i++)
		{
			if ( DriverStruct[i] == NULL )
				return DRIVER_STATUS_INITPEND;
			if (( DriverStruct[i]->status & DRIVER_STATUS_REQUESTED) ==  DRIVER_STATUS_REQUESTED)
			{
				DriverStruct[i]->status = DRIVER_STATUS_IN_USE;
				if ( DriverStruct[i]->init != NULL )
				{
					if ( (drv_ret = DriverStruct[i]->init(i)) == DRIVER_REQUEST_FAILED)
						DriverStruct[i]->status = DRIVER_STATUS_FAILED;
					else
					{
						if (( DriverStruct[i]->flags & DRIVER_FLAGS_AUTOSTART) == DRIVER_FLAGS_AUTOSTART)
						{
							if ( DriverStruct[i]->start != NULL )
								DriverStruct[i]->start(i);
						}

					}
				}
				driver_request --;
				if ( driver_request == 0)
					return drv_ret;
			}
		}
	}
	return DRIVER_STATUS_INITIALIZED;
}

uint32_t driver_start(uint32_t handle)
{
	if ( DriverStruct[handle]->start != NULL )
		return DriverStruct[handle]->start(handle);
	return DRIVER_REQUEST_FAILED;
}

uint32_t driver_get_values(uint32_t handle,uint8_t *values,uint8_t values_number)
{
	if ( DriverStruct[handle]->get_values != NULL )
		return DriverStruct[handle]->get_values(handle,values,values_number);
	return DRIVER_REQUEST_FAILED;
}

uint32_t driver_set_values(uint32_t handle,uint8_t *values,uint8_t values_number)
{
	if ( DriverStruct[handle]->set_values != NULL )
		return DriverStruct[handle]->set_values(handle,values,values_number);
	return DRIVER_REQUEST_FAILED;
}

uint32_t driver_extended_action(uint32_t handle,uint8_t action,uint32_t action_parameter,uint32_t extension_parameter)
{
	if ( DriverStruct[handle]->extended_action != NULL )
		return DriverStruct[handle]->extended_action(handle,action,action_parameter,extension_parameter);
	return DRIVER_REQUEST_FAILED;
}

uint32_t driver_get_handle_from_dma_channel(uint32_t *handle1 , uint32_t *handle2)
{
uint32_t	i,drv_ret=0;
BasicDriverStruct_t	*BasicDriverStruct;
	for(i=0;i<MAX_DRIVERS;i++)
	{
		if ( DriverStruct[i] != NULL )
		{
			if (( DriverStruct[i]->status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
			{
				if ( DriverStruct[i]->driver_private_data != NULL)
				{
					BasicDriverStruct = (BasicDriverStruct_t	*)DriverStruct[i]->driver_private_data;
					if ( BasicDriverStruct->hdma[0] != NULL)
					{
						*handle1 = i;
						drv_ret++;
					}
					if ( BasicDriverStruct->hdma[1] != NULL)
					{
						*handle2 = i;
						drv_ret++;
					}
				}
			}
		}
	}
	return drv_ret;
}

uint32_t 	driver_init(void)
{
uint32_t	i,drv_ret=0;
uint32_t size;

	size = sizeof(DriverStruct_t) / 4;

	for(i=0;i<MAX_DRIVERS;i++)
	{
		A_clear32((uint32_t	*)&DriverStruct[i],size);
	}
	return drv_ret;
}

