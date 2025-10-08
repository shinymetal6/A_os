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

void sample_process_1_gpio(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t state = 0;
	create_timer(TIMER_ID_0,500,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_TIM_IRQ | EVENT_EXT_INT_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & EVENT_TIMER) == EVENT_TIMER)
		{
			set_gpio_mode(DHT_DATA_GPIO_Port, DHT_DATA_Pin,state, PIN_MODE_OUTPUT, PIN_OTYPE_OPENDRAIN, PIN_PUPD_UP, PIN_SPEED_MEDIUM);
			//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, state);

			state ++;
			state &= 1;
		}
	}
}

#endif // #ifdef 	SAMPLEPROCESS_1_ENCODER
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
