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
 * gpio_int.c
 *
 *  Created on: Dec 7, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#include "gpio_int.h"

GPIO_Interrupt_DriverStruct_t	*gpio_irq_drv_ptr;

ITCM_AREA_CODE uint32_t	gpio_int_register(GPIO_Interrupt_DriverStruct_t *gpio_irq_Drv)
{
GPIO_Interrupt_DriverStruct_t *eptr, *pre_eptr;

	if ( gpio_irq_Drv->IRQ_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( gpio_irq_Drv->irq_exti_callback == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( gpio_irq_Drv == NULL)
	{
		gpio_irq_Drv = (GPIO_Interrupt_DriverStruct_t *)gpio_irq_Drv;
		gpio_irq_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = (GPIO_Interrupt_DriverStruct_t *)gpio_irq_Drv;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (GPIO_Interrupt_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)gpio_irq_Drv;
		gpio_irq_Drv->next_drv = NULL;
	}
	return 0;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
GPIO_Interrupt_DriverStruct_t	*gpio_irq_ptr_L = gpio_irq_drv_ptr;
	if ( gpio_irq_drv_ptr == NULL )
		return;
	while(gpio_irq_ptr_L->IRQ_bit != GPIO_Pin)
	{
		if ( gpio_irq_ptr_L->next_drv != NULL )
			gpio_irq_ptr_L = (GPIO_Interrupt_DriverStruct_t *)gpio_irq_ptr_L->next_drv;
		else
			return;
	}
	if ( gpio_irq_ptr_L->process != 0 )
	{
		if (( gpio_irq_ptr_L->IRQ_port != NULL ) && (gpio_irq_ptr_L->irq_exti_callback != NULL))
		{
			gpio_irq_ptr_L->irq_exti_callback(GPIO_Pin,gpio_irq_ptr_L->irq_origin_struct_ptr);
			if ((gpio_irq_ptr_L->flags  & GPIO_INT_WAKEUP_ON_EVENT) == GPIO_INT_WAKEUP_ON_EVENT)
				activate_process(gpio_irq_ptr_L->process,gpio_irq_ptr_L->wakeup_id,gpio_irq_ptr_L->wakeup_id);
		}
	}
}
