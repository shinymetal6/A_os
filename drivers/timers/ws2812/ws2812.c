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
 * ws2812.c
 *
 *  Created on: Feb 9, 2026
 *      Author: fil
 */

#include "main.h"

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

#include "ws2812.h"

extern	uint32_t ws2812_lut_rom[256][8];
__attribute__((section(".d2ram"))) __attribute__ ((aligned (32))) uint32_t ws2812_lut[256][8];

ITCM_AREA_CODE void ws2812_SetPixel(WS2812_DriverStruct_t *ws2812_drv,uint32_t location, uint8_t r,uint8_t g,uint8_t b)
{
int16_t	i;
	for(i=0;i<8;i++)
	{
		ws2812_drv->ws2812_work_buf[RED_SHIFT   + (location*24) + i] = ws2812_lut[r][i];
		ws2812_drv->ws2812_work_buf[GREEN_SHIFT + (location*24) + i] = ws2812_lut[g][i];
		ws2812_drv->ws2812_work_buf[BLUE_SHIFT  + (location*24) + i] = ws2812_lut[b][i];
	}
}

ITCM_AREA_CODE void ws2812_UserFB_to_WorkBuf(WS2812_DriverStruct_t *ws2812_drv,uint8_t *user_fb,uint32_t user_fb_len)
{
int32_t	i;
	if ( user_fb_len > ws2812_drv->ws2812_numleds)
		return;
	for(i=0;i<user_fb_len;i++)
		ws2812_SetPixel(ws2812_drv,i, user_fb[i],user_fb[i+1],user_fb[i+2]);
}

ITCM_AREA_CODE void ws2812_ClearPixels(WS2812_DriverStruct_t *ws2812_drv)
{
int16_t	location;
	for(location=0;location<ws2812_drv->ws2812_numleds;location++)
		ws2812_SetPixel(ws2812_drv,location, 0,0,0);
}

ITCM_AREA_CODE uint32_t ws2812_init(WS2812_DriverStruct_t *ws2812_drv)
{
	bzero(ws2812_drv->ws2812_work_buf,ws2812_drv->ws2812_work_buf_buflen);
	ws2812_ClearPixels(ws2812_drv);
	return HAL_TIM_PWM_Start_DMA(ws2812_drv->ws2812_timer, ws2812_drv->ws2812_timer_channel,ws2812_drv->ws2812_work_buf,ws2812_drv->ws2812_work_buf_buflen);
}

ITCM_AREA_CODE uint32_t	ws2812_register(WS2812_DriverStruct_t *ws2812_drv)
{
TIMER_DriverStruct_t *eptr, *pre_eptr;

	if ( ws2812_drv->ws2812_timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( ws2812_drv->ws2812_work_buf == NULL)
		return DRIVER_REQUEST_FAILED;
	if (( ws2812_drv->ws2812_work_buf_buflen == 0 ) || ( ws2812_drv->ws2812_work_buf_buflen > WS2812_MAX_BUFLEN ) )
		return DRIVER_REQUEST_FAILED;
	if ( ws2812_drv->ws2812_numleds > WS2812_MAX_NUMLEDS)
		return DRIVER_REQUEST_FAILED;

	if ( timer_drv_ptr == NULL)
	{
		timer_drv_ptr = (TIMER_DriverStruct_t *)ws2812_drv;
		ws2812_drv->next_timer = NULL;
	}
	else
	{
		eptr = pre_eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
		{
			pre_eptr = eptr;
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		}
		pre_eptr->next_timer = (uint32_t *)ws2812_drv;
		ws2812_drv->next_timer = NULL;
	}
	ws2812_drv->process = get_current_process();
	ws2812_drv->ws2812_timer->Instance->PSC = (HSI_CLOCK/(10000000U));
	ws2812_drv->ws2812_timer->Instance->ARR = 12;
	//A_copy32_32(&ws2812_lut_rom[0][0],&ws2812_lut[0][0],256*8);

	return ws2812_init(ws2812_drv);
}
#endif // #ifdef A_OS_TIMERS_ENABLED

