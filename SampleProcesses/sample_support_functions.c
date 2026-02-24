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
#include "../kernel/A_exported_functions.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED

uint8_t 	sample_led_cntr = 0;
#ifdef	STM32U575xx
#undef LED_GPIO_Port
#undef LED_Pin
/*
#define	LED_GPIO_Port	LED_GREEN_GPIO_Port
#define	LED_Pin			LED_GREEN_Pin
#define	LED_GPIO_Port	LED_RED_GPIO_Port
#define	LED_Pin			LED_RED_Pin
*/
#define	LED_GPIO_Port	LED_BLUE_GPIO_Port
#define	LED_Pin			LED_BLUE_Pin

#endif
#ifdef	STM32G474xx
#define	LED_GPIO_Port	GREEN_LED__GPIO_Port
#define	LED_Pin			GREEN_LED__Pin
#endif
#ifdef	STM32N657xx
#define	LED_GPIO_Port	LED1_GPIO_Port
#define	LED_Pin			LED1_Pin
#endif
#ifdef	STM32H563xx
#define	LED_GPIO_Port	LED0_GPIO_Port
#define	LED_Pin			LED0_Pin
#endif

void process_led(void)
{
#ifdef LED_GPIO_Port
	switch(sample_led_cntr)
	{
	case 7:
	case 9:
#ifdef AU100825
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
#else
#ifdef	STM32N657xx
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
#else
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
#endif
#endif
		break;
	default :
#ifdef AU100825
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
#else
#ifdef	STM32N657xx
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_SET);
#else
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
#endif
#endif
		break;
	}
	sample_led_cntr++;
	if ( sample_led_cntr >= 10 )
		sample_led_cntr = 0;
#endif // #ifdef LED_GPIO_Port
}

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

