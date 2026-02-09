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
 * sample_process_1_ws2812.c
 *
 *  Created on: Feb 9, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_WS2812

extern	TIM_HandleTypeDef htim3;

#define	NUM_LEDS		8
#define	WS2812_WORK_BUF_LEN		((NUM_LEDS*WS2812_LEDBPP)+WS2812_SYNCLEN)
uint32_t ws2812_work_buf[WS2812_WORK_BUF_LEN];

WS2812_Drv_TypeDef	WS2812_Drv =
{
		.ws2812_timer = &htim3,
		.ws2812_timer_channel = TIM_CHANNEL_1,
		.ws2812_numleds = 8,
		.ws2812_work_buf = &ws2812_work_buf[0],
		.ws2812_work_buf_buflen = WS2812_WORK_BUF_LEN,
		.wakeup_id = WAKEUP_FROM_TIM_IRQ,
};

uint8_t r,g,b,use_r,use_g,use_b;

void sample_process_1_init(uint32_t process_id)
{

}
void sample_process_1_ws2812(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	lednum=0;

	r=g=b= 0;
	use_r=use_g=use_b=0;
	use_r = 1;
	create_timer(TIMER_ID_0,20,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	ws2812_register(&WS2812_Drv);

	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			ws2812_SetPixel(&WS2812_Drv,lednum,r,g,b);
			if ( use_r )
			{
				r+=10;
				if ( r > 0x70 )
				{
					r = 0;
					use_r = 0;
					use_g = 1;
				}
			}
			if ( use_g )
			{
				g+=10;
				if ( g > 0x70 )
				{
					g = 0;
					use_g = 0;
					use_b = 1;
				}
			}
			if ( use_b )
			{
				b+=10;
				if ( b > 0x70 )
				{
					b = 0;
					use_b = 0;
					use_r = 1;
				}
			}
			lednum++;
			lednum &= 0x07;

		}
	}
}

#endif //#ifdef SAMPLEPROCESS_2_WS2812
#endif //#ifdef SAMPLE_PROCESSES_ENABLED


