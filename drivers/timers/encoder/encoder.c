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
 * encoder.c
 *
 *  Created on: Oct 6, 2025
 *      Author: fil
 */
#include "main.h"

#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
#ifdef A_OS_TIMERS_ENABLED
#include "encoder.h"
#include <string.h>

extern	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
extern	uint8_t				last_tim_used_handle,tim_driver_request;

ITCM_AREA_CODE uint32_t	encoder_register(Encoder_Drv_TypeDef *private_data)
{
Encoder_Drv_TypeDef	*encoder_driver_data;
	if ( TIM_DriverStruct[last_tim_used_handle].process == 0 )
	{
		TIM_DriverStruct[last_tim_used_handle].process = get_current_process();
		TIM_DriverStruct[last_tim_used_handle].flags = 0;
		TIM_DriverStruct[last_tim_used_handle].private_data = (uint32_t *)private_data;

		encoder_driver_data = (Encoder_Drv_TypeDef *)TIM_DriverStruct[last_tim_used_handle].private_data;
		if ( encoder_driver_data->encoder_timer == NULL )
			return DRIVER_REQUEST_FAILED;
		encoder_driver_data->flags = 0;
		TIM_DriverStruct[last_tim_used_handle].timer_type = TIM_TYPE_ENCODER;

		TIM_DriverStruct[last_tim_used_handle].status = DRIVER_STATUS_IN_USE;
		HAL_TIM_Encoder_Start_IT(encoder_driver_data->encoder_timer, TIM_CHANNEL_ALL);

		last_tim_used_handle++;
		tim_driver_request++;
		return last_tim_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

ITCM_AREA_CODE uint32_t get_handle_from_encoder_workers(TIM_HandleTypeDef 	*encoder_timer)
{
uint32_t	i,drv_ret=255;
Encoder_Drv_TypeDef	*encoder_driver_data;

	for(i=0;i<MAX_I2C_DEVICES;i++)
	{
		encoder_driver_data = (Encoder_Drv_TypeDef *)TIM_DriverStruct[i].private_data;

		if ( encoder_driver_data->encoder_timer == encoder_timer )
			return i;
	}
	return drv_ret;
}
#endif // #ifdef A_OS_TIMERS_ENABLED


