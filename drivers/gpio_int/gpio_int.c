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
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../kernel/kernel_opt.h"

#include "gpio_int.h"

SYSTEM_RAM	GPIO_Int_DriverStruct_t	GPIO_Int_DriverStruct[MAX_GPIOINT_DRIVERS];

ITCM_AREA_CODE void gpio_int_Driver_RxTimeoutCheckCallback(void)
{
uint32_t	i;
	__disable_irq();
	for(i=0;i<MAX_GPIOINT_DRIVERS;i++)
	{
		if ( GPIO_Int_DriverStruct[i].process )
		{
			if (( GPIO_Int_DriverStruct[i].flags & GPIO_INT_HIDE_ENABLED ) == GPIO_INT_HIDE_ENABLED )
			{
				if ( GPIO_Int_DriverStruct[i].hide_time_counter )
				{
					GPIO_Int_DriverStruct[i].hide_time_counter--;
					if ( GPIO_Int_DriverStruct[i].hide_time_counter == 0 )
					{
						GPIO_Int_DriverStruct[i].hide_time_counter = GPIO_Int_DriverStruct[i].hide_time;
						HAL_NVIC_ClearPendingIRQ(GPIO_Int_DriverStruct[i].irq_index);
						HAL_NVIC_EnableIRQ(GPIO_Int_DriverStruct[i].irq_index);
						GPIO_Int_DriverStruct[i].flags &= ~GPIO_INT_HIDE_ENABLED;
					}
				}
			}
		}
	}
	__enable_irq();
}

ITCM_AREA_CODE uint32_t	gpio_int_register(GPIO_Int_DriverStruct_t *driver_private_data)
{
uint32_t	i;
	for(i=0;i<MAX_GPIOINT_DRIVERS;i++)
	{
		if ( GPIO_Int_DriverStruct[i].process == 0 )
		{
			GPIO_Int_DriverStruct[i].flags = driver_private_data->flags;
			GPIO_Int_DriverStruct[i].irq_bit = driver_private_data->irq_bit;
			GPIO_Int_DriverStruct[i].wakeup_id = driver_private_data->wakeup_id;
			GPIO_Int_DriverStruct[i].irq_exti_callback = driver_private_data->irq_exti_callback;
			GPIO_Int_DriverStruct[i].hide_time = driver_private_data->hide_time;
			if ( GPIO_Int_DriverStruct[i].hide_time  )
			{
				GPIO_Int_DriverStruct[i].hide_time_counter = GPIO_Int_DriverStruct[i].hide_time;
				set_before_check_timers_callback(gpio_int_Driver_RxTimeoutCheckCallback);
				switch(GPIO_Int_DriverStruct[i].irq_bit)
				{
				case GPIO_PIN_0 : GPIO_Int_DriverStruct[i].irq_index = EXTI0_IRQn;break;
				case GPIO_PIN_1 : GPIO_Int_DriverStruct[i].irq_index = EXTI1_IRQn;break;
				case GPIO_PIN_2 : GPIO_Int_DriverStruct[i].irq_index = EXTI2_IRQn;break;
				case GPIO_PIN_3 : GPIO_Int_DriverStruct[i].irq_index = EXTI3_IRQn;break;
				}

			}
			if ( GPIO_Int_DriverStruct[i].flags != 0 )
			{
				if ( GPIO_Int_DriverStruct[i].wakeup_id == 0 )
					return DRIVER_REQUEST_FAILED;
			}
			if ( GPIO_Int_DriverStruct[i].irq_exti_callback == NULL )
				return DRIVER_REQUEST_FAILED;
			GPIO_Int_DriverStruct[i].process = get_current_process();
			return 0;
		}
		return 1;
	}
	return 1;
}

ITCM_AREA_CODE void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
uint32_t	i;
	for(i=0;i<MAX_GPIOINT_DRIVERS;i++)
	{
		if ( GPIO_Int_DriverStruct[i].irq_exti_callback == NULL)
			return;
		if ( GPIO_Int_DriverStruct[i].irq_bit == GPIO_Pin)
		{
			__disable_irq();
			GPIO_Int_DriverStruct[i].flags &= ~GPIO_INT_HIDE_ENABLED;
			if ( GPIO_Int_DriverStruct[i].hide_time )
			{
				HAL_NVIC_DisableIRQ(GPIO_Int_DriverStruct[i].irq_index);
				GPIO_Int_DriverStruct[i].flags |= GPIO_INT_HIDE_ENABLED;
			}
			GPIO_Int_DriverStruct[i].irq_exti_callback(GPIO_Pin);
			HAL_NVIC_ClearPendingIRQ(GPIO_Int_DriverStruct[i].irq_index);

			if ( GPIO_Int_DriverStruct[i].flags != 0 )
				activate_process(GPIO_Int_DriverStruct[i].process,GPIO_Int_DriverStruct[i].wakeup_id,GPIO_Pin);
			__enable_irq();
			return;
		}
	}
}
