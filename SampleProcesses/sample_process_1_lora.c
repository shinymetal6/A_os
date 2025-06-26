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
 * sample_process_1_lora.c
 *
 *  Created on: Jun 26, 2025
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_LORA
uint8_t		led_cntr=0;
#define	PRC1_TICK				10
extern	SPI_HandleTypeDef hspi1;

RA01S_Drv_TypeDef	RA01S_Drv =
{
		.spi = &hspi1,
		.CS_port = LORA_SS_GPIO_Port,
		.CS_bit = LORA_SS_Pin,
		.RESET_port = LORA_RESET_GPIO_Port,
		.RESET_bit = LORA_RESET_Pin,
		.wakeup_id = WAKEUP_FROM_SPI1_IRQ,
		.flags = 0,
};

void process_led(void)
{
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
	led_cntr++;
	if ( led_cntr == 100 )
		led_cntr = 0;
}
uint8_t	can_pkt = 0;

void sample_process_1_lora(uint32_t process_id)
{
uint32_t	wakeup,flags;
	ra01s_register(&RA01S_Drv);

	create_timer(TIMER_ID_0,PRC1_TICK,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);


		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if (( flags & TIMER_ID_0) == TIMER_ID_0)
				process_led();
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_LORA
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

