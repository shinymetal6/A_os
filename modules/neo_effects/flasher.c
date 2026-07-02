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
 * flasher.c
 *
 *  Created on: Jul 1, 2026
 *      Author: fil
 */

#include "main.h"

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

ITCM_AREA_CODE uint32_t	flasher_init(WS2812_DriverStruct_t *ws2812_drv,Flasher_Struct_t *Flasher)
{
	Flasher->flasher_len = ws2812_drv->ws2812_numleds-1;
	flasher_apply(ws2812_drv,Flasher,0);
	return 0;
}

ITCM_AREA_CODE uint32_t	flasher_apply(WS2812_DriverStruct_t *ws2812_drv,Flasher_Struct_t *Flasher,uint8_t state)
{
int32_t i;

	if ( state )
	{
		for(i=0;i<=Flasher->flasher_len;i++)
		{
			Flasher->led_buf[i].r = ((Flasher->r_on * Flasher->brightness) >> 8);
			Flasher->led_buf[i].g = ((Flasher->g_on * Flasher->brightness) >> 8);
			Flasher->led_buf[i].b = ((Flasher->b_on * Flasher->brightness) >> 8);
			ws2812_SetPixel(ws2812_drv,i,Flasher->led_buf[i].r,Flasher->led_buf[i].g,Flasher->led_buf[i].b);
			ws2812_update(ws2812_drv);
		}
	}
	else
	{
		for(i=0;i<=Flasher->flasher_len;i++)
		{
			Flasher->led_buf[i].r = ((Flasher->r_off * Flasher->brightness) >> 8);
			Flasher->led_buf[i].g = ((Flasher->g_off * Flasher->brightness) >> 8);
			Flasher->led_buf[i].b = ((Flasher->b_off * Flasher->brightness) >> 8);
			ws2812_SetPixel(ws2812_drv,i,Flasher->led_buf[i].r,Flasher->led_buf[i].g,Flasher->led_buf[i].b);
			ws2812_update(ws2812_drv);
		}
	}
	return 0;
}

