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
 * worm.c
 *
 *  Created on: Jun 30, 2026
 *      Author: fil
 */
#include "main.h"

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

ITCM_AREA_CODE uint32_t	worm_init(WS2812_DriverStruct_t *ws2812_drv,Worm_Struct_t *Worm)
{
int32_t i,k;
	Worm->worm_head = 0;
	for(i=0,k=7;i<=Worm->worm_len;i++,k--)
	{
		if ( Worm->direction == WORM_WS2812_COUNTER_CLOCKWISE )
		{
			Worm->led_buf[k].r = Worm->r >> i;
			Worm->led_buf[k].g = Worm->g >> i;
			Worm->led_buf[k].b = Worm->b >> i;
		}
		else
		{
			Worm->led_buf[i].r = Worm->r >> i;
			Worm->led_buf[i].g = Worm->g >> i;
			Worm->led_buf[i].b = Worm->b >> i;
		}
	}
	for(i=0;i<=Worm->worm_len;i++)
	{
		ws2812_SetPixel(ws2812_drv,i,Worm->led_buf[i].r,Worm->led_buf[i].g,Worm->led_buf[i].b);
		ws2812_update(ws2812_drv);
	}
	Worm->position = 0;
	return 0;
}

uint8_t lr,lg,lb;
ITCM_AREA_CODE uint32_t	worm_advance(WS2812_DriverStruct_t *ws2812_drv,Worm_Struct_t *Worm)
{
int32_t i,k;

	if ( Worm->direction == WORM_WS2812_COUNTER_CLOCKWISE )
	{
		Worm->position++;
		if ( Worm->position == Worm->worm_len )
			Worm->position = 0;
		for(i=0,k=Worm->position;i<=Worm->worm_len;i++,k++)
		{
			if ( k == Worm->worm_len)
				k = 0;
			ws2812_SetPixel(ws2812_drv,i,Worm->led_buf[k].r,Worm->led_buf[k].g,Worm->led_buf[k].b);
			ws2812_update(ws2812_drv);
		}
	}
	else
	{
		if ( Worm->position )
			Worm->position--;
		else
			Worm->position = Worm->worm_len-1;
		for(i=Worm->worm_len-1,k=Worm->position;i>=0;i--,k--)
		{
			if ( k < 0 )
				k = Worm->worm_len - 1;
			ws2812_SetPixel(ws2812_drv,i,Worm->led_buf[k].r,Worm->led_buf[k].g,Worm->led_buf[k].b);
			ws2812_update(ws2812_drv);
		}

	}
	return 0;
}

