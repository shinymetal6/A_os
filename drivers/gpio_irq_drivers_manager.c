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
#include "../kernel/kernel_opt.h"

#include <string.h>
#include "gpio_irq_drivers_manager.h"

extern	Asys_t		Asys;
extern		GPIO_Irq_DriverStruct_t	*GPIO_Irq_DriverStruct[MAX_GPIO_DRIVERS];

SYSTEM_RAM	uint8_t		last_gpio_used_handle=0,gpio_driver_gpio_request = 0;
SYSTEM_RAM	GPIO_Irq_DriverPortAllocationStruct_t	GPIO_Irq_DriverPortAllocation[MAX_GPIO_PORTS];

ITCM_AREA_CODE uint8_t gpio_driver_allocate_gpio(GPIO_TypeDef *GPIO_Port,uint16_t GPIO_Pin)
{
uint8_t		port_index;
uint32_t	gpio_port = (uint32_t )GPIO_Port;
	switch(gpio_port)
	{
	case	GPIOA_BASE : port_index=0;break;
	case	GPIOB_BASE : port_index=1;break;
	case	GPIOC_BASE : port_index=2;break;
	case	GPIOD_BASE : port_index=3;break;
	case	GPIOE_BASE : port_index=4;break;
	case	GPIOF_BASE : port_index=5;break;
	case	GPIOG_BASE : port_index=6;break;
	case	GPIOH_BASE : port_index=8;break;
	case	GPIOI_BASE : port_index=9;break;
	case	GPIOJ_BASE : port_index=10;break;
	case	GPIOK_BASE : port_index=11;break;
	default : return PIN_ALREADY_ALLOCATED;
	}
	if (( GPIO_Irq_DriverPortAllocation[port_index].gpiobit & (1 << GPIO_Pin)) == (1 << GPIO_Pin))
		return PIN_ALREADY_ALLOCATED;

	GPIO_Irq_DriverPortAllocation[port_index].in_use = 1;
	GPIO_Irq_DriverPortAllocation[port_index].gpiobit |= 1 << GPIO_Pin;
	return port_index;
}

ITCM_AREA_CODE uint32_t	gpio_driver_register(GPIO_Irq_DriverStruct_t *driver,uint32_t *private_drv_struct,uint32_t flags)
{
OnChip_GPIO_Irq_DriverStruct_t	*gpio_Drv;
	if ( GPIO_Irq_DriverStruct[last_gpio_used_handle] == NULL )
	{
		gpio_Drv = (OnChip_GPIO_Irq_DriverStruct_t *)private_drv_struct;
		if ( gpio_Drv == NULL )
			return DRIVER_STATUS_FAILED;

		if ( gpio_driver_allocate_gpio(gpio_Drv->GPIO_Port,gpio_Drv->GPIO_Pin) == PIN_ALREADY_ALLOCATED )
			return DRIVER_REQUEST_FAILED;
		gpio_Drv->flags |= flags;

		GPIO_Irq_DriverStruct[last_gpio_used_handle] = driver;
		GPIO_Irq_DriverStruct[last_gpio_used_handle]->process = get_current_process();
		GPIO_Irq_DriverStruct[last_gpio_used_handle]->gpio_driver_private_data = private_drv_struct;


		GPIO_Irq_DriverStruct[last_gpio_used_handle]->status = DRIVER_STATUS_REQUESTED;

		last_gpio_used_handle++;
		gpio_driver_gpio_request++;
		return last_gpio_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t	gpio_driver_unregister(GPIO_Irq_DriverStruct_t *driver)
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

ITCM_AREA_CODE uint32_t gpio_driver_scan(void)
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

ITCM_AREA_CODE uint32_t gpio_driver_gpio_set(uint32_t handle,uint8_t level)
{
	if ( handle > MAX_DRIVERS )
		return DRIVER_REQUEST_FAILED;
	if ( GPIO_Irq_DriverStruct[handle]->gpio_set != NULL )
		return GPIO_Irq_DriverStruct[handle]->gpio_set(handle,level);
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t gpio_driver_gpio_toggle(uint32_t handle)
{
	if ( handle > MAX_DRIVERS )
		return DRIVER_REQUEST_FAILED;
	if ( GPIO_Irq_DriverStruct[handle]->gpio_toggle != NULL )
		return GPIO_Irq_DriverStruct[handle]->gpio_toggle(handle);
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t gpio_driver_gpio_get(uint32_t handle)
{
	if ( handle > MAX_DRIVERS )
		return DRIVER_REQUEST_FAILED;
	if ( GPIO_Irq_DriverStruct[handle]->gpio_get != NULL )
		return GPIO_Irq_DriverStruct[handle]->gpio_get(handle);
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t gpio_driver_gpio_configure(uint32_t handle, uint8_t configuration)
{
	if ( handle > MAX_DRIVERS )
		return DRIVER_REQUEST_FAILED;
	if ( GPIO_Irq_DriverStruct[handle]->gpio_configure != NULL )
		return GPIO_Irq_DriverStruct[handle]->gpio_configure(handle,configuration);
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t gpio_driver_init(void)
{
uint32_t	i;

	for(i=0;i<MAX_GPIO_PORTS;i++)
		GPIO_Irq_DriverPortAllocation[i].in_use = GPIO_Irq_DriverPortAllocation[i].gpiobit=0;
	return 0;
}


