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

SYSTEM_RAM	GPIO_Int_DriverStruct_t	GPIO_Int_DriverStruct[MAX_GPIOINT_DRIVERS];

static GPIO_Interrupt_DriverStruct_t *get_private_from_gpio_int(uint16_t GPIO_Pin)
{
uint32_t	i;
GPIO_Interrupt_DriverStruct_t *private_data;

	for(i=0;i<MAX_GPIOINT_DRIVERS;i++)
	{
		if (( GPIO_Int_DriverStruct[i].status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
		{
			if ( GPIO_Int_DriverStruct[i].private_data != NULL )
			{
				private_data = (GPIO_Interrupt_DriverStruct_t	*)GPIO_Int_DriverStruct[i].private_data;
				if ( private_data->IRQ_bit == GPIO_Pin )
					return private_data;
			}
		}
	}
	return NULL;
}

static void gpio_int_Driver_RxTimeoutCheckCallback(void)
{
uint32_t	i;
GPIO_Interrupt_DriverStruct_t *private_data;
uint8_t bit_active = 0;

	__disable_irq();
	for(i=0;i<MAX_GPIOINT_DRIVERS;i++)
	{
		private_data = (GPIO_Interrupt_DriverStruct_t	*)GPIO_Int_DriverStruct[i].private_data;
		if ( private_data != NULL )
		{
			if (( private_data->debounce ) && (private_data->debounce_counter ))
			{
				if (( private_data->IRQ_type & GPIO_INT_TYPE_FALLING) == GPIO_INT_TYPE_FALLING)
				{
					if (HAL_GPIO_ReadPin(private_data->IRQ_port,private_data->IRQ_bit) == 0 )
						bit_active = 1;
				}
				if (( private_data->IRQ_type & GPIO_INT_TYPE_RISING) == GPIO_INT_TYPE_RISING)
				{
					if (HAL_GPIO_ReadPin(private_data->IRQ_port,private_data->IRQ_bit) == 1 )
						bit_active = 1;
				}
			}

			if ( bit_active )
			{
				private_data->debounce_counter --;
				if (( private_data->debounce ) && (private_data->debounce_counter == 0 ))
				{
					private_data->status |= GPIO_INT_EVENT;
					if ( private_data->wakeup_id != 0 )
						activate_process(private_data->process,private_data->wakeup_id,private_data->IRQ_bit);
					HAL_NVIC_ClearPendingIRQ(private_data->IRQ_bit);
					HAL_NVIC_EnableIRQ(private_data->IRQ_bit);
					if ( private_data->irq_exti_callback != NULL )
						private_data->irq_exti_callback(private_data->IRQ_bit);
				}
			}
			else
			{
				private_data->debounce_counter = 0;
				HAL_NVIC_ClearPendingIRQ(private_data->IRQ_bit);
				HAL_NVIC_EnableIRQ(private_data->IRQ_bit);
			}
		}
	}
	__enable_irq();
}
ITCM_AREA_CODE uint32_t	gpio_int_register(GPIO_Interrupt_DriverStruct_t *private_data)
{
uint32_t	i;
	for(i=0;i<MAX_GPIOINT_DRIVERS;i++)
	{
		if ( GPIO_Int_DriverStruct[i].process == 0 )
		{
			if ( private_data->IRQ_port == NULL )
				return DRIVER_REQUEST_FAILED;
			if (( private_data->wakeup_id == 0 ) && (private_data->irq_exti_callback == NULL ))
				return DRIVER_REQUEST_FAILED;
			if ( private_data->debounce )
			{
				private_data->debounce_counter = private_data->debounce;
				set_before_check_timers_callback(gpio_int_Driver_RxTimeoutCheckCallback);
			}
			GPIO_Int_DriverStruct[i].private_data = (uint32_t *)private_data;
			GPIO_Int_DriverStruct[i].process = private_data->process = get_current_process();
			GPIO_Int_DriverStruct[i].handle = i;
			GPIO_Int_DriverStruct[i].status = DRIVER_STATUS_IN_USE;
			return 0;
		}
	}
	return 1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	GPIO_Interrupt_DriverStruct_t *private_data = get_private_from_gpio_int(GPIO_Pin);
	if ( private_data != NULL )
	{
		if (( private_data->IRQ_bit == GPIO_Pin) && ( private_data->IRQ_port != NULL ))
		{
			HAL_NVIC_DisableIRQ(private_data->IRQ_bit);
			private_data->sampled_bit = HAL_GPIO_ReadPin(private_data->IRQ_port,private_data->IRQ_bit);
			if ( private_data->debounce == 0 )
			{
				private_data->status |= GPIO_INT_EVENT;
				if (( private_data->flags  & GPIO_INT_WAKEUP_ON_EVENT ) == GPIO_INT_WAKEUP_ON_EVENT )
					if ( private_data->wakeup_id != 0 )
						activate_process(private_data->process,private_data->wakeup_id,private_data->IRQ_bit);
				HAL_NVIC_ClearPendingIRQ(private_data->IRQ_bit);
				HAL_NVIC_EnableIRQ(private_data->IRQ_bit);
				if ( private_data->irq_exti_callback != NULL )
					private_data->irq_exti_callback(private_data->IRQ_bit);
			}
			else
				private_data->debounce_counter = private_data->debounce;
		}
	}
}
