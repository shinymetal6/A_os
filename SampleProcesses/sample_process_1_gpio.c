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
 * sample_process_1_gpio.c
 *
 *  Created on: Oct 7, 2025
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef 	SAMPLEPROCESS_1_GPIO

extern	void irq_exti_callback(uint16_t GPIO_Pin,uint32_t *irq_origin_struct_ptr);

GPIO_Interrupt_DriverStruct_t	gpio_irq_Drv =
{
		.IRQ_port = IRQ_D2_GPIO_Port,
		.IRQ_bit = IRQ_D2_Pin,
		.irq_exti_callback = irq_exti_callback,
		.wakeup_id = WAKEUP_FROM_EXT_INT_IRQ,
		.flags = GPIO_INT_WAKEUP_ON_EVENT,
};

void irq_exti_callback(uint16_t GPIO_Pin,uint32_t *irq_origin_struct_ptr)
{
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
}

void sample_process_1_gpio(uint32_t process_id)
{
uint32_t	wakeup,flags;

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	gpio_int_register(&gpio_irq_Drv);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_EXT_INT_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & EVENT_TIMER) == EVENT_TIMER)
			process_led();
		if (( wakeup & EVENT_EXT_INT_IRQ) == EVENT_EXT_INT_IRQ)
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
}

#endif // #ifdef 	SAMPLEPROCESS_1_ENCODER
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
