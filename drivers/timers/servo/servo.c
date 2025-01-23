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
 * servo.c
 *
 *  Created on: Jan 23, 2025
 *      Author: fil
 */

#include "main.h"

#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"
#ifdef A_OS_TIMERS_ENABLED

#include "servo.h"
#include <string.h>

extern	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
extern	uint8_t				last_tim_used_handle,tim_driver_request;

ITCM_AREA_CODE uint32_t servo_start(uint8_t handle)
{
SERVO_Control_Drv_TypeDef	*servo_driver_data = (SERVO_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;
	HAL_TIM_PWM_Start(servo_driver_data->servo_timer,servo_driver_data->servo_channel);
	__HAL_TIM_DISABLE(servo_driver_data->servo_timer);
	return 0;
}

ITCM_AREA_CODE uint32_t servo_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE uint32_t servo_get_status(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE uint32_t servo_set_prescaler(uint8_t handle,uint32_t prescaler)
{
	return 0;
}

ITCM_AREA_CODE uint32_t servo_set_position(uint8_t handle,uint8_t servo_position,uint8_t servo_pulses)
{
	// position is expressed in percentage : 0 .. 100
	if ( servo_position > 100 )
		return DRIVER_STATUS_FAILED;

	SERVO_Control_Drv_TypeDef	*servo_driver_data = (SERVO_Control_Drv_TypeDef *)TIM_DriverStruct[handle].private_data;
	__HAL_TIM_DISABLE(servo_driver_data->servo_timer);
	servo_driver_data->servo_timer->Instance->CNT = 0;
	servo_driver_data->servo_timer->Instance->ARR = servo_driver_data->cycle_time;
	servo_driver_data->servo_timer->Instance->CCR1 = servo_driver_data->cycle_time - 1000 - (servo_position * 10);
	servo_driver_data->servo_timer->Instance->RCR = servo_driver_data->repetition;
    __HAL_TIM_ENABLE(servo_driver_data->servo_timer);

	//HAL_TIM_PWM_Start(servo_driver_data->servo_timer,servo_driver_data->servo_channel);
	return 0;
}

ITCM_AREA_CODE uint32_t	servo_register(SERVO_Control_Drv_TypeDef *private_data)
{
SERVO_Control_Drv_TypeDef	*servo_driver_data;
	if ( TIM_DriverStruct[last_tim_used_handle].process == 0 )
	{
		TIM_DriverStruct[last_tim_used_handle].process = get_current_process();
		TIM_DriverStruct[last_tim_used_handle].flags = 0;
		TIM_DriverStruct[last_tim_used_handle].private_data = (uint32_t *)private_data;

		servo_driver_data = (SERVO_Control_Drv_TypeDef *)TIM_DriverStruct[last_tim_used_handle].private_data;
		if ( servo_driver_data->servo_timer == NULL )
			return DRIVER_REQUEST_FAILED;
		if ( servo_driver_data->repetition == 0 )
			servo_driver_data->repetition = 7;
		else
			servo_driver_data->repetition --;
		servo_driver_data->servo_timer->Instance->ARR = servo_driver_data->cycle_time;
		servo_driver_data->flags = 0;
		TIM_DriverStruct[last_tim_used_handle].status = DRIVER_STATUS_IN_USE;

		last_tim_used_handle++;
		tim_driver_request++;
		return last_tim_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

#endif // #ifdef A_OS_TIMERS_ENABLED
