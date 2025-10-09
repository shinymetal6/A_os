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
 *  Created on: Feb 24, 2025
 *      Author: fil
 */

#include "main.h"

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

#include "ws2812.h"

extern	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
extern	uint8_t				last_tim_used_handle,tim_driver_request;

__attribute__	((aligned (16)))	uint16_t	ws2812_FrameBuffer[BUFLEN][LEDBPP]; /* 0GRB */

ITCM_AREA_CODE static void ws2812_compile_sync(void)
{
int16_t	i,k,location;

	for(location=0;location<SYNCLEN;location++)
	{
		for(i=0,k=7;i<8;i++,k--)
		{
			ws2812_FrameBuffer[location][k] = 1;
			ws2812_FrameBuffer[location][k+8] = 1;
			ws2812_FrameBuffer[location][k+16] = 1;
		}
	}
}

ITCM_AREA_CODE void ws2812_CompilePixel(uint32_t location, uint8_t r,uint8_t g,uint8_t b)
{
int16_t	i,k;
	for(i=0,k=7;i<8;i++,k--)
	{
		if ( (g & (1 << i)) != 0 )
			ws2812_FrameBuffer[SYNCLEN+location][k] = PATTERN_1;
		else
			ws2812_FrameBuffer[SYNCLEN+location][k] = PATTERN_0;
		if ( (r & (1 << i)) != 0 )
			ws2812_FrameBuffer[SYNCLEN+location][k+8] = PATTERN_1;
		else
			ws2812_FrameBuffer[SYNCLEN+location][k+8] = PATTERN_0;
		if ( (b & (1 << i)) != 0 )
			ws2812_FrameBuffer[SYNCLEN+location][k+16] = PATTERN_1;
		else
			ws2812_FrameBuffer[SYNCLEN+location][k+16] = PATTERN_0;
	}
	ws2812_compile_sync();
}

ITCM_AREA_CODE uint32_t ws2812_init(uint8_t handle)
{
int16_t	location;
WS2812_Drv_TypeDef	*ws2812_drv = (WS2812_Drv_TypeDef *)TIM_DriverStruct[last_tim_used_handle].private_data;

	for(location=SYNCLEN;location<SYNCLEN+NUMLEDS;location++)
		ws2812_CompilePixel(location, 0,0,0);
	HAL_TIM_PWM_Start_DMA(ws2812_drv->ws2812_timer, ws2812_drv->ws2812_channel,(uint32_t *)ws2812_FrameBuffer,BUFLEN);
	return 0;
}

ITCM_AREA_CODE uint32_t	ws2812_register(WS2812_Drv_TypeDef *private_data)
{
	if ( TIM_DriverStruct[last_tim_used_handle].process == 0 )
	{
		TIM_DriverStruct[last_tim_used_handle].process = get_current_process();
		TIM_DriverStruct[last_tim_used_handle].private_data = (uint32_t *)private_data;

		WS2812_Drv_TypeDef	*ws2812_drv = (WS2812_Drv_TypeDef *)TIM_DriverStruct[last_tim_used_handle].private_data;
		if ( ws2812_drv->ws2812_timer == NULL )
			return DRIVER_REQUEST_FAILED;
		TIM_DriverStruct[last_tim_used_handle].status = DRIVER_STATUS_IN_USE;

		last_tim_used_handle++;
		tim_driver_request++;
		return last_tim_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}
#endif // #ifdef A_OS_TIMERS_ENABLED
