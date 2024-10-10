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
 * drivers_common.c
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
#include "drivers_common.h"

SYSTEM_RAM	DriversDefs_t	DriversDefs[NUM_MAX_DRIVERS];

uint32_t	driver_register(DriversDefs_t *driver)
{
uint32_t	i;
	for(i=0;i<NUM_MAX_DRIVERS;i++)
	{
		if ( DriversDefs[i].process == 0 )
		{
			DriversDefs[i].process = get_current_process();
			DriversDefs[i].status = DRIVER_IN_USE;
			DriversDefs[i].flags = 0;
			if (driver->before_check_timers_callback != NULL  )
			{
				DriversDefs[i].before_check_timers_callback = driver->before_check_timers_callback;
				set_before_check_timers_callback(driver->before_check_timers_callback);
			}
			if (driver->after_check_timers_callback != NULL  )
			{
				DriversDefs[i].after_check_timers_callback = driver->after_check_timers_callback;
				set_after_check_timers_callback(driver->after_check_timers_callback);
			}
			strcpy(DriversDefs[i].driver_name,driver->driver_name);
			return 0;
		}
	}
	return 1;
}

uint32_t	driver_unregister(DriversDefs_t *driver)
{
uint32_t	i;
	for(i=0;i<NUM_MAX_DRIVERS;i++)
	{
		if ( strcmp(DriversDefs[i].driver_name,driver->driver_name) == 0 )
		{
			DriversDefs[i].process = DriversDefs[i].status = DriversDefs[i].flags = DRIVER_UNUSED;
			DriversDefs[i].before_check_timers_callback = NULL;
			DriversDefs[i].after_check_timers_callback = NULL;
			return 0;
		}
	}
	return 1;
}



