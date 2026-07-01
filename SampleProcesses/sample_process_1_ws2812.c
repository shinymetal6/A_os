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

extern	TIM_HandleTypeDef htim16;

#define	NUM_LEDS		8
#define WS2812_BUF_SIZE 		(WS2812_RESET_HEAD + (NUM_LEDS * WS2812_LEDBPP) + WS2812_RESET_TAIL)

uint16_t ws2812_work_buf[WS2812_BUF_SIZE];
uint32_t worm_led_buf[NUM_LEDS];
uint32_t glow_led_buf[NUM_LEDS];
Worm_Struct_t		WormBuf =
{
		.led_buf = worm_led_buf,
		.r = 0,
		.g = 0,
		.b = 255,
		.direction = 1,
		.worm_len = NUM_LEDS,
};

Glow_Struct_t		GlowBuf =
{
		.led_buf = glow_led_buf,
		.r = 0,
		.g = 0,
		.b = 255,
		.direction = 1,
		.initial_brightness = 0,
		.glow_step = 4,
		.glow_len = NUM_LEDS,
};

WS2812_DriverStruct_t	WS2812_Drv =
{
		.ws2812_timer = &htim16,
		.ws2812_timer_channel = TIM_CHANNEL_1,
		.ws2812_numleds = NUM_LEDS,
		.ws2812_work_buf = &ws2812_work_buf[0],
		.ws2812_work_buf_buflen = WS2812_BUF_SIZE,
		.ws2812_arr_val = 211,
		.wakeup_id = WAKEUP_FROM_TIM_IRQ,
};

uint8_t r,g,b,use_r,use_g,use_b;

void sample_process_1_init(uint32_t process_id)
{

}

//#define	WORM_EFFECT	1
#define	GLOW_EFFECT	1

void sample_process_1_ws2812(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	lednum=0 , startws=0 , up=1;

	r=g=b= 0;
	use_r=use_g=use_b=0;
	use_r = 1;
	create_timer(TIMER_ID_0,50,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	ws2812_register(&WS2812_Drv);
#ifdef WORM_EFFECT
	worm_init(&WS2812_Drv,&WormBuf);
#endif //#ifdef WORM_EFFECT
#ifdef GLOW_EFFECT
	glow_init(&WS2812_Drv,&GlowBuf);

#endif //#ifdef GLOW_EFFECT

	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_TIM_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			if ( startws == 0 )
			{
				ws2812_SetPixel(&WS2812_Drv,lednum,r,g,b);
				ws2812_update(&WS2812_Drv);
				startws=1;
			}
			else
			{
#ifdef WORM_EFFECT
				worm_advance(&WS2812_Drv,&WormBuf);
#endif //#ifdef WORM_EFFECT
#ifdef GLOW_EFFECT
				glow_apply(&WS2812_Drv,&GlowBuf);
#endif //#ifdef GLOW_EFFECT
			}
		}
		if (( wakeup & WAKEUP_FROM_TIM_IRQ) == WAKEUP_FROM_TIM_IRQ)
		{

		}
	}
}

#endif //#ifdef SAMPLEPROCESS_2_WS2812
#endif //#ifdef SAMPLE_PROCESSES_ENABLED


