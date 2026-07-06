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

ITCM_AREA_CODE void ws2812_SetPixel(WS2812_DriverStruct_t *ws2812_drv,uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
uint32_t color;
uint16_t buffer_start_index = WS2812_RESET_HEAD + (index * 24);
uint16_t buf_idx;

    if (index >= ws2812_drv->ws2812_numleds)
    {
    	return;
    }

    if ( ws2812_drv->ws2812_type == 0 )
    	color = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b; // GRB (Standard WS2812)
    else
    	color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b; // RGB (WS2811 or some clones)

    for (int i = 23; i >= 0; i--)
    {
		buf_idx = buffer_start_index + (23 - i);
		if ((color >> i) & 1)
			ws2812_drv->ws2812_work_buf[buf_idx] = WS2812_1; // '1' duty cycle
		else
			ws2812_drv->ws2812_work_buf[buf_idx] = WS2812_0;  // '0' duty cycle

    }
}

ITCM_AREA_CODE void ws2812_ClearPixels(WS2812_DriverStruct_t *ws2812_drv)
{
uint32_t i;
    // 1. Clear the HEAD with pure 0s (Strictly LOW for Reset Pulse)
    for (i = 0; i < WS2812_RESET_HEAD; i++) {
    	ws2812_drv->ws2812_work_buf[i] = 0;
    }

    // 2. Clear the DATA section with '0' bits (Duty cycle 68)
    for (i = WS2812_RESET_HEAD; i < WS2812_RESET_HEAD + (ws2812_drv->ws2812_numleds * 24); i++) {
    	ws2812_drv->ws2812_work_buf[i] = WS2812_0;
    }

    // 3. Clear the TAIL with pure 0s (Strictly LOW to end the frame cleanly)
    for (i = WS2812_RESET_HEAD + (ws2812_drv->ws2812_numleds * 24); i < ws2812_drv->ws2812_work_buf_buflen; i++) {
    	ws2812_drv->ws2812_work_buf[i] = 0;
    }
}

ITCM_AREA_CODE uint32_t ws2812_update(WS2812_DriverStruct_t *ws2812_drv)
{
	ws2812_drv->ws2812_timer->Instance->CCR1 = WS2812_0;
    return HAL_TIM_PWM_Start_DMA(ws2812_drv->ws2812_timer, ws2812_drv->ws2812_timer_channel,(uint32_t*)ws2812_drv->ws2812_work_buf,ws2812_drv->ws2812_work_buf_buflen);
}

ITCM_AREA_CODE uint32_t ws2812_init(WS2812_DriverStruct_t *ws2812_drv)
{
	bzero(ws2812_drv->ws2812_work_buf,ws2812_drv->ws2812_work_buf_buflen);
	ws2812_ClearPixels(ws2812_drv);
	return HAL_TIM_PWM_Start_DMA(ws2812_drv->ws2812_timer, ws2812_drv->ws2812_timer_channel,(uint32_t*)ws2812_drv->ws2812_work_buf,ws2812_drv->ws2812_work_buf_buflen);
}

ITCM_AREA_CODE uint32_t	ws2812_register(WS2812_DriverStruct_t *ws2812_drv)
{
TIMER_DriverStruct_t *eptr;
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
		eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		eptr->next_timer = (uint32_t *)ws2812_drv;
		ws2812_drv->next_timer = NULL;
	}

	ws2812_drv->process = get_current_process();
	ws2812_drv->timer_type = TIM_TYPE_PWM;
	if ( ws2812_drv->ws2812_one_val == 0 )
		ws2812_drv->ws2812_one_val = WS2812_1;
	if ( ws2812_drv->ws2812_zero_val == 0 )
		ws2812_drv->ws2812_zero_val = WS2812_0;
	if ( ws2812_drv->ws2812_arr_val != 0 )
		ws2812_drv->ws2812_timer->Instance->ARR = ws2812_drv->ws2812_arr_val;
	return ws2812_init(ws2812_drv);
}
#endif // #ifdef A_OS_TIMERS_ENABLED

