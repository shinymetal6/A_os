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

uint8_t		led_cntr=0;

void process_led(void)
{
	/*
	switch(led_cntr)
	{
	case 70 :
	case 90 :
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
		break;
	default :
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
		break;
	}
	*/
#ifdef	STM32H753xx
	#ifdef STM32H753ZI_NUCLEO
	if (((led_cntr>=70) && (led_cntr<=80)) || ((led_cntr>=90) && (led_cntr<=100)))
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
	#endif // #ifdef STM32H753ZI_NUCLEO
#else
	if (((led_cntr>=70) && (led_cntr<=80)) || ((led_cntr>=90) && (led_cntr<=100)))
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
#endif // #ifdef	STM32H753xx

	led_cntr++;
	if ( led_cntr == 100 )
		led_cntr = 0;
}

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

