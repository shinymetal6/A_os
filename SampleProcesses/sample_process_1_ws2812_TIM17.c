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
 * sample_process_1_ws2812_TIM17.c
 *
 *  Created on: Oct 20, 2026
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_WS2812_TIM17


uint16_t 				ws2812_work_buf[WS2812_DMA_BUF_SIZE];
LED_Frame_Struct_t		LED_Frame[WS2812_NUM_LEDS];

extern	TIM_HandleTypeDef htim17;

WS2812_DriverStruct_t	WS2812_Drv =
{
	.wakeup_id 			 = WAKEUP_FROM_TIM_IRQ,
	.ws2812_timer        = &htim17,
	.tim_channel         = TIM_CHANNEL_1,
	.num_leds            = WS2812_NUM_LEDS,
	.dma_buf_size        = WS2812_DMA_BUF_SIZE,
	.dma_pwm_buffer      = ws2812_work_buf,
	.led_strip_data		 = LED_Frame,
	.brightness			 = 0x3f,
};

void sample_process_1_ws2812_TIM17_init(uint32_t process_id)
{
	ws2812_register(&WS2812_Drv);
}

void sample_process_1_ws2812_TIM17(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	ledlit=0;

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	LED_Frame[ledlit] = (LED_Frame_Struct_t){0, 0, 255}; // Set first pixel Blue
	ws2812_Show_Frame(&WS2812_Drv);
	ledlit++;

	while(1)
	{
		wait_event(EVENT_TIMER| EVENT_TIM_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
		}
		if (( wakeup & WAKEUP_FROM_TIM_IRQ) == WAKEUP_FROM_TIM_IRQ)
		{
			if (WS2812_Drv.transmitting == 0 )
			{
				if ( ledlit == 0 )
					LED_Frame[WS2812_NUM_LEDS-1] = (LED_Frame_Struct_t){0, 0, 0}; // Clear last pixel
				else
					LED_Frame[ledlit-1] = (LED_Frame_Struct_t){0, 0, 0}; // Clear last pixel
				/*
				ws2812_SetLED(&WS2812_Drv,ledlit,255, 0,0);
				ws2812_SetLED(&WS2812_Drv,ledlit,0, 255, 0);
				*/
				ws2812_SetLED(&WS2812_Drv,ledlit,0,0, 255);
				ws2812_Show_Frame(&WS2812_Drv);
				ledlit++;
				if ( ledlit >= WS2812_NUM_LEDS)
				{
					ledlit=0;
					WS2812_Drv.brightness+=10;
					if ( WS2812_Drv.brightness > 250 )
						WS2812_Drv.brightness = 0;
				}
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_WS2812_TIM17
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
