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

SYSTEM_RAM	DriversDefs_t	*DriversDefs[MAX_DRIVERS];
SYSTEM_RAM	uint8_t			last_used_handle=0,driver_request = 0;

uint32_t	driver_register(DriversDefs_t *driver,uint32_t flags)
{
	if ( DriversDefs[last_used_handle] == NULL )
	{
		DriversDefs[last_used_handle] = driver;
		DriversDefs[last_used_handle]->process = get_current_process();
		DriversDefs[last_used_handle]->flags |= flags;

		if (driver->periodic_before_check_timers_callback != NULL  )
			set_before_check_timers_callback(driver->periodic_before_check_timers_callback);
		if (driver->periodic_after_check_timers_callback != NULL  )
			set_after_check_timers_callback(driver->periodic_after_check_timers_callback);
		DriversDefs[last_used_handle]->status = DRIVER_STATUS_REQUESTED;

		last_used_handle++;
		driver_request++;
		return last_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

uint32_t	driver_unregister(DriversDefs_t *driver)
{
uint32_t	i;
	for(i=0;i<MAX_DRIVERS;i++)
	{
		if ( strcmp(DriversDefs[i]->driver_name,driver->driver_name) == 0 )
		{
			DriversDefs[i] = NULL;
			last_used_handle--;
			return 0;
		}
	}
	return 1;
}

uint32_t driver_init(void)
{
uint32_t	i,drv_ret;
	if (driver_request )
	{
		for(i=0;i<MAX_DRIVERS;i++)
		{
			if (( DriversDefs[i]->status & DRIVER_STATUS_REQUESTED) ==  DRIVER_STATUS_REQUESTED)
			{
				DriversDefs[i]->status = DRIVER_STATUS_IN_USE;
				if ( DriversDefs[i]->init != NULL )
				{
					if ( (drv_ret = DriversDefs[i]->init(i)) == DRIVER_REQUEST_FAILED)
						DriversDefs[i]->status = DRIVER_STATUS_FAILED;
					else
					{
						if (( DriversDefs[i]->flags & DRIVER_FLAGS_AUTOSTART) == DRIVER_FLAGS_AUTOSTART)
						{
							if ( DriversDefs[i]->start != NULL )
								DriversDefs[i]->start(i);
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
	if ( DriversDefs[handle]->start != NULL )
		return DriversDefs[handle]->start(handle);
	return DRIVER_REQUEST_FAILED;
}

uint32_t driver_get_values(uint32_t handle,uint8_t *values,uint8_t values_number)
{
	if ( DriversDefs[handle]->get_values != NULL )
		return DriversDefs[handle]->get_values(handle,values,values_number);
	return DRIVER_REQUEST_FAILED;
}

uint32_t driver_set_values(uint32_t handle,uint8_t *values,uint8_t values_number)
{
	if ( DriversDefs[handle]->set_values != NULL )
		return DriversDefs[handle]->set_values(handle,values,values_number);
	return DRIVER_REQUEST_FAILED;
}

uint32_t driver_extended_action(uint32_t handle,uint8_t action,uint32_t action_parameter,uint32_t extension_parameter)
{
	if ( DriversDefs[handle]->extended_action != NULL )
		return DriversDefs[handle]->extended_action(handle,action,action_parameter,extension_parameter);
	return DRIVER_REQUEST_FAILED;
}

