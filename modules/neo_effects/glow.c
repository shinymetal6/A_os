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
 * glow.c
 *
 *  Created on: Jul 1, 2026
 *      Author: fil
 */

#include "main.h"

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

ITCM_AREA_CODE uint32_t	glow_init(WS2812_DriverStruct_t *ws2812_drv,Glow_Struct_t *Glow)
{
int32_t i;
	Glow->glow_len = ws2812_drv->ws2812_numleds-1;
	Glow->current_brightness = Glow->initial_brightness;
	for(i=0;i<Glow->glow_len;i++)
	{
		Glow->led_buf[i].r = ((Glow->r * Glow->current_brightness) >> 8);
		Glow->led_buf[i].g = ((Glow->g * Glow->current_brightness) >> 8);
		Glow->led_buf[i].b = ((Glow->b * Glow->current_brightness) >> 8);
		ws2812_SetPixel(ws2812_drv,i,Glow->led_buf[i].r,Glow->led_buf[i].g,Glow->led_buf[i].b);
		ws2812_update(ws2812_drv);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t	glow_apply(WS2812_DriverStruct_t *ws2812_drv,Glow_Struct_t *Glow)
{
int32_t i;


	if ( Glow->glow_mode == WS2812_GLOW_ONLY_UP)
	{
		for(i=0;i<=Glow->glow_len;i++)
		{
			if (( Glow->current_brightness + Glow->glow_step) < Glow->final_brightness)
				Glow->current_brightness += Glow->glow_step;
			else
				Glow->current_brightness = Glow->initial_brightness;

			Glow->led_buf[i].r = ((Glow->r * Glow->current_brightness) >> 8);
			Glow->led_buf[i].g = ((Glow->g * Glow->current_brightness) >> 8);
			Glow->led_buf[i].b = ((Glow->b * Glow->current_brightness) >> 8);
		}
	}

	else if ( Glow->glow_mode == WS2812_GLOW_ONLY_DOWN)
	{
		for(i=0;i<=Glow->glow_len;i++)
		{
			if ( Glow->current_brightness > (Glow->initial_brightness + Glow->glow_step))
				Glow->current_brightness -= Glow->glow_step;
			else
				Glow->current_brightness = Glow->final_brightness;

			Glow->led_buf[i].r = ((Glow->r * Glow->current_brightness) >> 8);
			Glow->led_buf[i].g = ((Glow->g * Glow->current_brightness) >> 8);
			Glow->led_buf[i].b = ((Glow->b * Glow->current_brightness) >> 8);
		}
	}

	else if ( Glow->glow_mode == WS2812_GLOW_BIDIRECTIONAL)
	{
		for(i=0;i<=Glow->glow_len;i++)
		{
			if ( Glow->glow_direction == WS2812_GLOW_UP)
			{
				if (( Glow->current_brightness + Glow->glow_step) < Glow->final_brightness)
					Glow->current_brightness += Glow->glow_step;
				else
					Glow->glow_direction = WS2812_GLOW_DOWN;
			}
			else
			{
				if ( Glow->current_brightness > (Glow->initial_brightness + Glow->glow_step))
					Glow->current_brightness -= Glow->glow_step;
				else
					Glow->glow_direction = WS2812_GLOW_UP;
			}
			Glow->led_buf[i].r = ((Glow->r * Glow->current_brightness) >> 8);
			Glow->led_buf[i].g = ((Glow->g * Glow->current_brightness) >> 8);
			Glow->led_buf[i].b = ((Glow->b * Glow->current_brightness) >> 8);
		}
	}
	else
		return 1;
	for(i=0;i<=Glow->glow_len;i++)
	{
		ws2812_SetPixel(ws2812_drv,i,Glow->led_buf[i].r,Glow->led_buf[i].g,Glow->led_buf[i].b);
		ws2812_update(ws2812_drv);
	}
	return 0;
}
#endif //#ifdef A_OS_TIMERS_ENABLED

