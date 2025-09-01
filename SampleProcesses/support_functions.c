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
 * support_functions.c
 *
 *  Created on: Jun 26, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED

uint8_t 	led_cntr = 0;

void process_led(void)
{
	switch(led_cntr)
	{
	case 7:
	case 9:
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
		break;
	default :
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
		break;
	}
	led_cntr++;
	if ( led_cntr >= 10 )
		led_cntr = 0;
}

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

