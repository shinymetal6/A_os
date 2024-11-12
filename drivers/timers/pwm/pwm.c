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
 * pwm.c
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
#include "../../../kernel/kernel_opt.h"

#include "pwm.h"
#include <string.h>

extern	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
extern	uint8_t				last_tim_used_handle,tim_driver_request;

 uint32_t pwm_start(uint8_t handle,uint32_t pwm_channel)
{
Pwm_Control_TypeDef	*pwm_drv = (Pwm_Control_TypeDef *)TIM_DriverStruct[handle].tim_driver_private_data;
TIM_HandleTypeDef	*timer = pwm_drv->pwm_timer;

	switch(pwm_channel)
	{
	case	TIM_CHANNEL_1	:	timer->Instance->CCR1 = pwm_drv->pulse_width; break;
	case	TIM_CHANNEL_2	:	timer->Instance->CCR2 = pwm_drv->pulse_width; break;
	case	TIM_CHANNEL_3	:	timer->Instance->CCR3 = pwm_drv->pulse_width; break;
	case	TIM_CHANNEL_4	:	timer->Instance->CCR4 = pwm_drv->pulse_width; break;
	default : return 1;
	}
	if ( HAL_TIM_PWM_Start(timer,pwm_channel) == 0 )
		pwm_drv->status |= PWM_CHANNEL_RUNNING;
	return 0;
}

 uint32_t pwm_stop(uint8_t handle)
{
Pwm_Control_TypeDef	*pwm_drv = (Pwm_Control_TypeDef *)TIM_DriverStruct[handle].tim_driver_private_data;
	if ( HAL_TIM_PWM_Stop(pwm_drv->pwm_timer,pwm_drv->pwm_channel) == 0 )
		pwm_drv->status &= ~PWM_CHANNEL_RUNNING;
	return 0;
}

 uint32_t pwm_get_status(uint8_t handle)
{
Pwm_Control_TypeDef	*pwm_drv = (Pwm_Control_TypeDef *)TIM_DriverStruct[handle].tim_driver_private_data;
	return pwm_drv->status;
}

uint32_t pwm_set_prescaler(uint8_t handle,uint32_t prescaler)
{
Pwm_Control_TypeDef	*pwm_drv = (Pwm_Control_TypeDef *)TIM_DriverStruct[handle].tim_driver_private_data;
TIM_HandleTypeDef	*timer = pwm_drv->pwm_timer;
	pwm_drv->prescaler = timer->Instance->PSC = prescaler;
	return 0;
}

uint32_t pwm_set_width(uint8_t handle,uint32_t pulse_width,uint32_t pwm_channel)
{
Pwm_Control_TypeDef	*pwm_drv = (Pwm_Control_TypeDef *)TIM_DriverStruct[handle].tim_driver_private_data;
TIM_HandleTypeDef	*timer = pwm_drv->pwm_timer;
	switch(pwm_channel)
	{
	case	TIM_CHANNEL_1	:	pwm_drv->pulse_width = timer->Instance->CCR1 = pulse_width; break;
	case	TIM_CHANNEL_2	:	pwm_drv->pulse_width = timer->Instance->CCR2 = pulse_width; break;
	case	TIM_CHANNEL_3	:	pwm_drv->pulse_width = timer->Instance->CCR3 = pulse_width; break;
	case	TIM_CHANNEL_4	:	pwm_drv->pulse_width = timer->Instance->CCR4 = pulse_width; break;
	default : return 1;
	}
	return 0;
}

uint32_t pwm_set_direction(uint8_t handle,uint8_t pwm_direction)
{
Pwm_Control_TypeDef	*pwm_drv = (Pwm_Control_TypeDef *)TIM_DriverStruct[handle].tim_driver_private_data;
	if ( pwm_drv->enable_port != NULL )
	{
		pwm_drv->pwm_direction = pwm_direction;
		if ( pwm_direction )
			pwm_drv->enable_port->BSRR = pwm_drv->enable_bit;
		else
			pwm_drv->enable_port->BSRR = (uint32_t)pwm_drv->enable_bit << 16;
		return 0;
	}
	else
		return 1;
}

 uint32_t pwm_init(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE uint32_t	pwm_register(Pwm_Control_TypeDef *tim_driver_private_data,uint32_t driver_flags,uint32_t timer_flags)
{
Pwm_Control_TypeDef	*dcc_driver_data;
	if ( TIM_DriverStruct[last_tim_used_handle].process == 0 )
	{
		TIM_DriverStruct[last_tim_used_handle].process = get_current_process();
		TIM_DriverStruct[last_tim_used_handle].flags |= driver_flags;
		TIM_DriverStruct[last_tim_used_handle].tim_driver_private_data = (uint32_t *)tim_driver_private_data;

		dcc_driver_data = (Pwm_Control_TypeDef *)TIM_DriverStruct[last_tim_used_handle].tim_driver_private_data;
		dcc_driver_data->flags |= timer_flags;
		TIM_DriverStruct[last_tim_used_handle].status = DRIVER_STATUS_REQUESTED;

		last_tim_used_handle++;
		tim_driver_request++;
		return last_tim_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}


