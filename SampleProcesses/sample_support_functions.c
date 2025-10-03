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
 * sample_support_functions.c
 *
 *  Created on: Jun 26, 2025
 *      Author: fil
 */

#include "main.h"
#include "../kernel/A.h"
#include "../kernel/system_default.h"
#include "../kernel/A_exported_functions.h"
#include <stdio.h>
#include <string.h>
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED

uint8_t 	sample_led_cntr = 0;

void process_led(void)
{
#ifdef LED_GPIO_Port
	switch(sample_led_cntr)
	{
	case 7:
	case 9:
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
		break;
	default :
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
		break;
	}
	sample_led_cntr++;
	if ( sample_led_cntr >= 10 )
		sample_led_cntr = 0;
#endif // #ifdef LED_GPIO_Port
#ifdef LED_RED_GPIO_Port
	switch(sample_led_cntr)
	{
	case 7:
	case 9:
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin,GPIO_PIN_SET);
		break;
	default :
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin,GPIO_PIN_RESET);
		break;
	}
	sample_led_cntr++;
	if ( sample_led_cntr >= 10 )
		sample_led_cntr = 0;
#endif // #ifdef LED_GPIO_Port
}

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

