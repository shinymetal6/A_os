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
 * gpio_irq.c
 *
 *  Created on: Oct 29, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#include "gpio_irq.h"
#include <string.h>

extern		GPIO_Irq_DriverStruct_t	*GPIO_Irq_DriverStruct[MAX_GPIO_DRIVERS];

static uint32_t driver_gpio_set(uint8_t handle, uint8_t level)
{
OnChip_GPIO_Irq_DriverStruct_t	*gpio_Drv;
	gpio_Drv = (OnChip_GPIO_Irq_DriverStruct_t *)GPIO_Irq_DriverStruct[handle]->gpio_driver_private_data;
	if (level != 0)
		gpio_Drv->GPIO_Port->BSRR = gpio_Drv->GPIO_Pin;
	else
		gpio_Drv->GPIO_Port->BSRR = (uint32_t)gpio_Drv->GPIO_Pin << 16;
	return 0;
}

static uint32_t driver_gpio_get(uint8_t handle)
{
OnChip_GPIO_Irq_DriverStruct_t	*gpio_Drv;
	gpio_Drv = (OnChip_GPIO_Irq_DriverStruct_t *)GPIO_Irq_DriverStruct[handle]->gpio_driver_private_data;
	return ((gpio_Drv->GPIO_Port->IDR & gpio_Drv->GPIO_Pin) ? 1 : 0);
}

static uint32_t driver_gpio_configure(uint8_t handle, uint8_t configuration)
{
OnChip_GPIO_Irq_DriverStruct_t	*gpio_Drv;
	gpio_Drv = (OnChip_GPIO_Irq_DriverStruct_t *)GPIO_Irq_DriverStruct[handle]->gpio_driver_private_data;
	if ( (configuration & GPIO_HAS_PULLUP) == GPIO_HAS_PULLUP)
	{
		gpio_Drv->GPIO_Port->PUPDR |= 1 << (((gpio_Drv->GPIO_Pin+1) * 2)-2);
	}
	if ( (configuration & GPIO_HAS_PULLOWN) == GPIO_HAS_PULLOWN)
	{
		gpio_Drv->GPIO_Port->PUPDR |= 1 << (((gpio_Drv->GPIO_Pin+1) * 2)-1);
	}
	if ( (configuration & GPIO_IS_TP) == GPIO_IS_TP)
	{
		gpio_Drv->GPIO_Port->OTYPER &= ~gpio_Drv->GPIO_Pin;
	}
	if ( (configuration & GPIO_IS_OC) == GPIO_IS_OC)
	{
		gpio_Drv->GPIO_Port->OTYPER |= gpio_Drv->GPIO_Pin;
	}
	if ( (configuration & GPIO_IS_ALTERNATE) == GPIO_IS_ALTERNATE)
	{
		gpio_Drv->GPIO_Port->OTYPER |= gpio_Drv->GPIO_Pin;
	}

	switch( configuration & GPIO_DRV_MODEMASK)
	{
	case	GPIO_IS_INPUT 		:
		gpio_Drv->GPIO_Port->MODER &= ~(0x03 << (gpio_Drv->GPIO_Pin<< 1));
		gpio_Drv->GPIO_Port->PUPDR &= ~(0x03 << (gpio_Drv->GPIO_Pin<< 1));
		break;
	case	GPIO_IS_ALTERNATE 	:
		gpio_Drv->GPIO_Port->MODER &= ~(0x03 << (gpio_Drv->GPIO_Pin<< 1));
		gpio_Drv->GPIO_Port->MODER |= 1 << (((gpio_Drv->GPIO_Pin+1) * 2)-1);
		break;
	case	GPIO_IS_OUTPUT	 	:
		gpio_Drv->GPIO_Port->MODER &= ~(0x03 << (gpio_Drv->GPIO_Pin<< 1));
		gpio_Drv->GPIO_Port->MODER |= 1 << (((gpio_Drv->GPIO_Pin+1) * 2)-2);
		break;
	case	GPIO_IS_ANALOG	 	:
		gpio_Drv->GPIO_Port->MODER |= 0x03 << (gpio_Drv->GPIO_Pin << 1);
		break;
	default	 	: break;
	}
	switch( configuration & GPIO_DRV_SPEEDMASK)
	{
	case GPIO_DRV_SPEED_LOW		: gpio_Drv->GPIO_Port->OSPEEDR |= 1 << (((gpio_Drv->GPIO_Pin+1) * 2)-1); break; // 10 -> low
	case GPIO_DRV_SPEED_MEDIUM	: gpio_Drv->GPIO_Port->OSPEEDR |= 1 << (((gpio_Drv->GPIO_Pin+1) * 2)-2); break; // 01 -> medium
	case GPIO_DRV_SPEED_HIGH	: gpio_Drv->GPIO_Port->OSPEEDR |= (0x03 << (gpio_Drv->GPIO_Pin<< 1));    break; // 11 -> high
	default						: gpio_Drv->GPIO_Port->OSPEEDR |= 1 << (((gpio_Drv->GPIO_Pin+1) * 2)-1); break; // 10 -> low
	}

	return 0;
}

GPIO_Irq_DriverStruct_t	Gpio_Drv =
{
	.gpio_set = driver_gpio_set,
	.gpio_get = driver_gpio_get,
	.gpio_configure = driver_gpio_configure,
	.gpio_driver_name = "driver_gpio",
};
/*
 * 		if (driver->periodic_before_check_timers_callback != NULL  )
			set_before_check_timers_callback(driver->periodic_before_check_timers_callback);
		if (driver->periodic_after_check_timers_callback != NULL  )
			set_after_check_timers_callback(driver->periodic_after_check_timers_callback);
 */
uint32_t driver_gpio_allocate_driver(GPIO_Irq_DriverStruct_t *new_struct)
{
	memcpy(new_struct,&Gpio_Drv,sizeof(Gpio_Drv));
	return 0;
}

static uint8_t find_handle_from_gpio(uint16_t GPIO_Pin)
{
uint8_t	i;
OnChip_GPIO_Irq_DriverStruct_t	*gpio_Drv;

	for(i=0;i<MAX_GPIO_DRIVERS;i++)
	{
		gpio_Drv = (OnChip_GPIO_Irq_DriverStruct_t *)GPIO_Irq_DriverStruct[i]->gpio_driver_private_data;
		if ( GPIO_Pin == gpio_Drv->GPIO_Pin)
			return i;
	}
	return 255;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
uint8_t handle;
OnChip_GPIO_Irq_DriverStruct_t	*gpio_Drv;
	handle = find_handle_from_gpio(GPIO_Pin);
	gpio_Drv = (OnChip_GPIO_Irq_DriverStruct_t *)GPIO_Irq_DriverStruct[handle]->gpio_driver_private_data;
	if ( gpio_Drv->exti_irq_callback != NULL )
		return gpio_Drv->exti_irq_callback(GPIO_Pin);
}


