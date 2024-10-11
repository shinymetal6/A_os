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
 *  Created on: Oct 10, 2024
 *      Author: fil
 */

#include "main.h"
#include "../kernel/system_default.h"
#include "../kernel/A.h"
#include "../kernel/A_exported_functions.h"
#include "../kernel/scheduler.h"
#include <string.h>
#include "drivers_manager.h"

SYSTEM_RAM	DriversDefsSensors_t	DriversDefsSensors[MAX_SENSORS_DRIVERS];
SYSTEM_RAM	uint8_t					last_used_handle=0,driver_request = 0;

uint32_t	sensors_driver_register(DriversDefsSensors_t *driver)
{
	if ( DriversDefsSensors[last_used_handle].process == 0 )
	{
		DriversDefsSensors[last_used_handle].process = get_current_process();
		DriversDefsSensors[last_used_handle].status = DRIVER_REQUESTED;
		DriversDefsSensors[last_used_handle].flags = 0;
		DriversDefsSensors[last_used_handle].handle = last_used_handle;
		DriversDefsSensors[last_used_handle].init = driver->init;
		DriversDefsSensors[last_used_handle].start = driver->start;
		DriversDefsSensors[last_used_handle].get_values = driver->get_values;
		DriversDefsSensors[last_used_handle].process = get_current_process();
		if (driver->periodic_before_check_timers_callback != NULL  )
		{
			DriversDefsSensors[last_used_handle].periodic_before_check_timers_callback = driver->periodic_before_check_timers_callback;
			set_before_check_timers_callback(driver->periodic_before_check_timers_callback);
		}
		if (driver->periodic_after_check_timers_callback != NULL  )
		{
			DriversDefsSensors[last_used_handle].periodic_after_check_timers_callback = driver->periodic_after_check_timers_callback;
			set_after_check_timers_callback(driver->periodic_after_check_timers_callback);
		}
		strcpy(DriversDefsSensors[last_used_handle].driver_name,driver->driver_name);
		last_used_handle++;
		driver_request = 1;
		return last_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

uint32_t	sensors_driver_unregister(DriversDefsSensors_t *driver)
{
uint32_t	i;
	for(i=0;i<MAX_SENSORS_DRIVERS;i++)
	{
		if ( strcmp(DriversDefsSensors[i].driver_name,driver->driver_name) == 0 )
		{
			DriversDefsSensors[i].process = DriversDefsSensors[i].status = DriversDefsSensors[i].flags = DRIVER_UNUSED;
			DriversDefsSensors[i].periodic_before_check_timers_callback = NULL;
			DriversDefsSensors[i].periodic_after_check_timers_callback = NULL;
			last_used_handle--;
			return 0;
		}
	}
	return 1;
}

uint32_t sensors_driver_get_values(uint32_t handle,uint8_t *values)
{
	if ( DriversDefsSensors[handle].get_values != NULL )
		return DriversDefsSensors[handle].get_values(values);
	return DRIVER_REQUEST_FAILED;
}

uint32_t sensors_driver_init(void)
{
uint32_t	i,drv_ret;
	if (driver_request )
	{
		for(i=0;i<MAX_SENSORS_DRIVERS;i++)
		{
			if (( DriversDefsSensors[i].status & DRIVER_REQUESTED) ==  DRIVER_REQUESTED)
			{
				DriversDefsSensors[i].status = DRIVER_IN_USE;
				if ( DriversDefsSensors[i].init != NULL )
				{
					if ( (drv_ret = DriversDefsSensors[i].init()) == DRIVER_REQUEST_FAILED)
						DriversDefsSensors[i].status = DRIVER_FAILED;
				}
				driver_request --;
				if ( driver_request == 0)
					return drv_ret;
			}
		}
	}
	return DRIVER_INITIALIZED;
}

uint32_t sensors_driver_start(uint32_t handle)
{
	if ( DriversDefsSensors[handle].start != NULL )
		return DriversDefsSensors[handle].start();
	return DRIVER_REQUEST_FAILED;
}




