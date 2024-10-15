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

/*******************************************/
/****		SENSORS SECTION 			****/
/*******************************************/

SYSTEM_RAM	DriversDefsSensors_t	DriversDefsSensors[MAX_SENSORS_DRIVERS];
SYSTEM_RAM	uint8_t					last_used_sensors_handle=0,sensors_driver_request = 0;

uint32_t	sensors_driver_register(DriversDefsSensors_t *driver,uint32_t flags)
{
	if ( DriversDefsSensors[last_used_sensors_handle].process == 0 )
	{
		DriversDefsSensors[last_used_sensors_handle].process = get_current_process();
		DriversDefsSensors[last_used_sensors_handle].status = DRIVER_STATUS_REQUESTED;
		DriversDefsSensors[last_used_sensors_handle].flags = driver->flags | flags;
		DriversDefsSensors[last_used_sensors_handle].handle = last_used_sensors_handle;
		DriversDefsSensors[last_used_sensors_handle].peripheral = driver->peripheral;
		DriversDefsSensors[last_used_sensors_handle].init = driver->init;
		DriversDefsSensors[last_used_sensors_handle].start = driver->start;
		DriversDefsSensors[last_used_sensors_handle].get_values = driver->get_values;
		DriversDefsSensors[last_used_sensors_handle].process = get_current_process();
		if (driver->periodic_before_check_timers_callback != NULL  )
		{
			DriversDefsSensors[last_used_sensors_handle].periodic_before_check_timers_callback = driver->periodic_before_check_timers_callback;
			set_before_check_timers_callback(driver->periodic_before_check_timers_callback);
		}
		if (driver->periodic_after_check_timers_callback != NULL  )
		{
			DriversDefsSensors[last_used_sensors_handle].periodic_after_check_timers_callback = driver->periodic_after_check_timers_callback;
			set_after_check_timers_callback(driver->periodic_after_check_timers_callback);
		}
		strcpy(DriversDefsSensors[last_used_sensors_handle].driver_name,driver->driver_name);
		last_used_sensors_handle++;
		sensors_driver_request = 1;
		return last_used_sensors_handle-1;
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
			DriversDefsSensors[i].process = DriversDefsSensors[i].status = DriversDefsSensors[i].flags = DRIVER_STATUS_UNUSED;
			DriversDefsSensors[i].periodic_before_check_timers_callback = NULL;
			DriversDefsSensors[i].periodic_after_check_timers_callback = NULL;
			last_used_sensors_handle--;
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
	if (sensors_driver_request )
	{
		for(i=0;i<MAX_SENSORS_DRIVERS;i++)
		{
			if (( DriversDefsSensors[i].status & DRIVER_STATUS_REQUESTED) ==  DRIVER_STATUS_REQUESTED)
			{
				DriversDefsSensors[i].status = DRIVER_STATUS_IN_USE;
				if ( DriversDefsSensors[i].init != NULL )
				{
					if ( (drv_ret = DriversDefsSensors[i].init()) == DRIVER_REQUEST_FAILED)
						DriversDefsSensors[i].status = DRIVER_STATUS_FAILED;
				}
				sensors_driver_request --;
				if ( sensors_driver_request == 0)
					return drv_ret;
			}
		}
	}
	return DRIVER_STATUS_INITIALIZED;
}

uint32_t sensors_driver_start(uint32_t handle)
{
	if ( DriversDefsSensors[handle].start != NULL )
		return DriversDefsSensors[handle].start();
	return DRIVER_REQUEST_FAILED;
}

/*******************************************/
/****		ACTUATORS SECTION 			****/
/*******************************************/

SYSTEM_RAM	DriversDefsActuators_t	DriversDefsActuators[MAX_SENSORS_DRIVERS];
SYSTEM_RAM	uint8_t					last_used_actuators_handle=0,actuators_driver_request = 0;

uint32_t	actuators_driver_register(DriversDefsActuators_t *driver,uint32_t flags)
{
	if ( driver != NULL )
	{
		if ( DriversDefsActuators[last_used_actuators_handle].process == 0 )
		{
			DriversDefsActuators[last_used_actuators_handle].process = get_current_process();
			DriversDefsActuators[last_used_actuators_handle].status = DRIVER_STATUS_REQUESTED;
			DriversDefsActuators[last_used_actuators_handle].flags = driver->flags | flags;
			DriversDefsActuators[last_used_actuators_handle].handle = last_used_actuators_handle;
			DriversDefsActuators[last_used_actuators_handle].peripheral = driver->peripheral;
			DriversDefsActuators[last_used_actuators_handle].init = driver->init;
			DriversDefsActuators[last_used_actuators_handle].start = driver->start;
			DriversDefsActuators[last_used_actuators_handle].get_values = driver->get_values;
			DriversDefsActuators[last_used_actuators_handle].process = get_current_process();
			if (driver->periodic_before_check_timers_callback != NULL  )
			{
				DriversDefsActuators[last_used_actuators_handle].periodic_before_check_timers_callback = driver->periodic_before_check_timers_callback;
				set_before_check_timers_callback(driver->periodic_before_check_timers_callback);
			}
			if (driver->periodic_after_check_timers_callback != NULL  )
			{
				DriversDefsActuators[last_used_actuators_handle].periodic_after_check_timers_callback = driver->periodic_after_check_timers_callback;
				set_after_check_timers_callback(driver->periodic_after_check_timers_callback);
			}
			strcpy(DriversDefsActuators[last_used_actuators_handle].driver_name,driver->driver_name);
			DriversDefsActuators[last_used_actuators_handle].extended_action = driver->extended_action;

			last_used_actuators_handle++;
			actuators_driver_request = 1;
			return last_used_actuators_handle-1;
		}
	}
	return DRIVER_REQUEST_FAILED;
}

uint32_t	actuators_driver_unregister(DriversDefsActuators_t *driver)
{
uint32_t	i;
	for(i=0;i<MAX_SENSORS_DRIVERS;i++)
	{
		if ( strcmp(DriversDefsActuators[i].driver_name,driver->driver_name) == 0 )
		{
			DriversDefsActuators[i].process = DriversDefsActuators[i].status = DriversDefsActuators[i].flags = DRIVER_STATUS_UNUSED;
			DriversDefsActuators[i].periodic_before_check_timers_callback = NULL;
			DriversDefsActuators[i].periodic_after_check_timers_callback = NULL;
			last_used_actuators_handle--;
			return 0;
		}
	}
	return 1;
}

uint32_t actuators_driver_get_values(uint32_t handle,uint8_t *values)
{
	if ( DriversDefsActuators[handle].get_values != NULL )
		return DriversDefsActuators[handle].get_values(values);
	return DRIVER_REQUEST_FAILED;
}

uint32_t actuators_driver_init(void)
{
uint32_t	i,drv_ret;
	if (actuators_driver_request )
	{
		for(i=0;i<MAX_SENSORS_DRIVERS;i++)
		{
			if (( DriversDefsActuators[i].status & DRIVER_STATUS_REQUESTED) ==  DRIVER_STATUS_REQUESTED)
			{
				DriversDefsActuators[i].status |= DRIVER_STATUS_IN_USE;
				if ( DriversDefsActuators[i].init != NULL )
				{
					if ( (drv_ret = DriversDefsActuators[i].init()) == DRIVER_REQUEST_FAILED)
						DriversDefsActuators[i].status = DRIVER_STATUS_FAILED;
					else
					{
						if (( DriversDefsActuators[i].flags & DRIVER_FLAGS_AUTOSTART) == DRIVER_FLAGS_AUTOSTART)
						{
							if ( DriversDefsActuators[i].start != NULL )
								DriversDefsActuators[i].start();
						}

					}
				}
				actuators_driver_request --;
				if ( actuators_driver_request == 0)
					return drv_ret;
			}
		}
	}
	return DRIVER_STATUS_INITIALIZED;
}

uint32_t actuators_driver_start(uint32_t handle)
{
	if ( DriversDefsActuators[handle].start != NULL )
		return DriversDefsActuators[handle].start();
	return DRIVER_REQUEST_FAILED;
}

uint32_t actuators_driver_extended_action(uint32_t handle,uint8_t action,uint32_t action_parameter,uint32_t extension_parameter)
{
	if ( DriversDefsActuators[handle].extended_action != NULL )
		return DriversDefsActuators[handle].extended_action(action,action_parameter,extension_parameter);
	return DRIVER_REQUEST_FAILED;
}




