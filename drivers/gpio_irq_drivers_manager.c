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
 * gpio_irq_drivers_manager.c
 *
 *  Created on: Oct 29, 2024
 *      Author: fil
 */

#include "main.h"
#include "../kernel/system_default.h"
#include "../kernel/A.h"
#include "../kernel/A_exported_functions.h"
#include "../kernel/scheduler.h"
#include <string.h>
#include "gpio_irq_drivers_manager.h"

extern		GPIO_Irq_DriverStruct_t	*GPIO_Irq_DriverStruct[MAX_GPIO_DRIVERS];
SYSTEM_RAM	uint8_t			last_gpio_used_handle=0,gpio_driver_gpio_request = 0;

uint32_t	gpio_driver_register(GPIO_Irq_DriverStruct_t *driver,uint32_t *private_drv_struct,uint32_t flags)
{
	if ( GPIO_Irq_DriverStruct[last_gpio_used_handle] == NULL )
	{
		GPIO_Irq_DriverStruct[last_gpio_used_handle] = driver;
		GPIO_Irq_DriverStruct[last_gpio_used_handle]->process = get_current_process();
		GPIO_Irq_DriverStruct[last_gpio_used_handle]->flags |= flags;
		GPIO_Irq_DriverStruct[last_gpio_used_handle]->gpio_driver_private_data = private_drv_struct;


		GPIO_Irq_DriverStruct[last_gpio_used_handle]->status = DRIVER_STATUS_REQUESTED;

		last_gpio_used_handle++;
		gpio_driver_gpio_request++;
		return last_gpio_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

uint32_t	gpio_driver_unregister(GPIO_Irq_DriverStruct_t *driver)
{
uint32_t	i;
	for(i=0;i<MAX_DRIVERS;i++)
	{
		if ( strcmp(GPIO_Irq_DriverStruct[i]->gpio_driver_name,driver->gpio_driver_name) == 0 )
		{
			GPIO_Irq_DriverStruct[i] = NULL;
			last_gpio_used_handle--;
			return 0;
		}
	}
	return 1;
}

uint32_t gpio_driver_scan(void)
{
uint32_t	i;
	if (gpio_driver_gpio_request )
	{
		for(i=0;i<MAX_DRIVERS;i++)
		{
			if ( GPIO_Irq_DriverStruct[i] == NULL )
				return DRIVER_STATUS_INITPEND;
			if (( GPIO_Irq_DriverStruct[i]->status & DRIVER_STATUS_REQUESTED) ==  DRIVER_STATUS_REQUESTED)
			{
				GPIO_Irq_DriverStruct[i]->status = DRIVER_STATUS_IN_USE;
				gpio_driver_gpio_request --;
				if ( gpio_driver_gpio_request == 0)
					return DRIVER_STATUS_UNUSED;
			}
		}
	}
	return DRIVER_STATUS_INITIALIZED;
}

uint32_t gpio_driver_gpio_set(uint32_t handle,uint8_t level)
{
	if ( GPIO_Irq_DriverStruct[handle]->gpio_set != NULL )
		return GPIO_Irq_DriverStruct[handle]->gpio_set(handle,level);
	return DRIVER_REQUEST_FAILED;
}

uint32_t gpio_driver_gpio_get(uint32_t handle)
{
	if ( GPIO_Irq_DriverStruct[handle]->gpio_get != NULL )
		return GPIO_Irq_DriverStruct[handle]->gpio_get(handle);
	return DRIVER_REQUEST_FAILED;
}

uint32_t gpio_driver_gpio_configure(uint32_t handle, uint8_t configuration)
{
	if ( GPIO_Irq_DriverStruct[handle]->gpio_configure != NULL )
		return GPIO_Irq_DriverStruct[handle]->gpio_configure(handle,configuration);
	return DRIVER_REQUEST_FAILED;
}

uint32_t 	gpio_driver_init(void)
{
uint32_t	i,drv_ret=0;
uint32_t size;

	size = sizeof(GPIO_Irq_DriverStruct_t) / 4;

	for(i=0;i<MAX_DRIVERS;i++)
	{
		A_clear32((uint32_t	*)&GPIO_Irq_DriverStruct[i],size);
	}
	return drv_ret;
}


