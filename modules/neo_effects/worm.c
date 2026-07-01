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

Worm_Struct_t	Worm;
ITCM_AREA_CODE uint32_t	worm_init(WS2812_DriverStruct_t *ws2812_drv,Worm_Struct_t *Worm)
{
int32_t i;
	Worm->worm_head = ws2812_drv->ws2812_numleds-1;
	if ( Worm->direction == 0 )
	{
		for(i=Worm->worm_head;i>=0;i--)
			Worm->led_buf[i] = (Worm->r >> (Worm->worm_head-i)) <<16 |	(Worm->g >> (Worm->worm_head-i)) << 8 | (Worm->b >> (Worm->worm_head-i));
	}
	else
	{
		for(i=0;i<=Worm->worm_head;i++)
			Worm->led_buf[i] = (Worm->r >> (Worm->worm_head-i)) <<16 |	(Worm->g >> (Worm->worm_head-i)) << 8 | (Worm->b >> (Worm->worm_head-i));
	}
	Worm->position = 0;
	return 0;
}

ITCM_AREA_CODE uint32_t	worm_advance(WS2812_DriverStruct_t *ws2812_drv,Worm_Struct_t *Worm)
{
int32_t i;
	if ( Worm->direction == 0 )
	{
		for(i=Worm->worm_head;i>=0;i--)
		{
			ws2812_SetPixel(ws2812_drv,(Worm->position+i)&(Worm->worm_head),Worm->led_buf[i]>>16,Worm->led_buf[i]>>8,Worm->led_buf[i]);
			ws2812_update(ws2812_drv);
		}
		Worm->position++;
		if ( Worm->position > Worm->worm_head)
			Worm->position = 0;
	}
	else
	{
		for(i=0;i<=Worm->worm_head;i++)
		{
			ws2812_SetPixel(ws2812_drv,(Worm->position+i)&(Worm->worm_head),Worm->led_buf[i]>>16,Worm->led_buf[i]>>8,Worm->led_buf[i]);
			ws2812_update(ws2812_drv);
		}
		Worm->position--;
		if ( Worm->position == 0 )
			Worm->position = Worm->worm_head;
	}
	//Worm->position &= Worm->worm_head;
	return 0;
}

