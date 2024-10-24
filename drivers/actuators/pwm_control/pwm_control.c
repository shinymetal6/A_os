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
 * pwm_control.c
 *
 *  Created on: Oct 14, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#ifdef A_HAS_MOTOR_CNTRL
#include "pwm_control.h"
#include <string.h>

extern	DriverStruct_t			*DriverStruct[MAX_DRIVERS];

static uint32_t pwm_control_start(uint8_t handle)
{
Pwm_Control_Drv_TypeDef	*Pwm_Control_Drv = (Pwm_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;

	if ( HAL_TIM_PWM_Start(Pwm_Control_Drv->pwm_timer,Pwm_Control_Drv->pwm_channel) == 0 )
		Pwm_Control_Drv->status |= PWM_CHANNEL_RUNNING;
	return 0;
}

static uint32_t pwm_control_stop(uint8_t handle)
{
Pwm_Control_Drv_TypeDef	*Pwm_Control_Drv = (Pwm_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	if ( HAL_TIM_PWM_Stop(Pwm_Control_Drv->pwm_timer,Pwm_Control_Drv->pwm_channel) == 0 )
		Pwm_Control_Drv->status &= ~PWM_CHANNEL_RUNNING;
	return 0;
}

static uint32_t pwm_control_get_status(uint8_t handle)
{
Pwm_Control_Drv_TypeDef	*Pwm_Control_Drv = (Pwm_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	return Pwm_Control_Drv->status;
}

static uint32_t pwm_control_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t pwm_control_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t pwm_control_extended_actions(uint32_t handle,uint32_t *action)
{
Pwm_Control_Drv_TypeDef	*Pwm_Control_Drv = (Pwm_Control_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
TIM_HandleTypeDef		*timer = Pwm_Control_Drv->pwm_timer;
Pwm_Control_Actions_TypeDef	*action_struct = (Pwm_Control_Actions_TypeDef *)&action;

	switch ( action_struct->action )
	{
	case	PWM_EA_SET_PWM_PRESCALER:
		timer->Instance->PSC = action_struct->prescaler;
		break;
	case	PWM_EA_SET_PWM_PERIOD:
		switch(Pwm_Control_Drv->pwm_channel)
		{
		case	TIM_CHANNEL_1	:	timer->Instance->CCR1 = action_struct->pulse_width; break;
		case	TIM_CHANNEL_2	:	timer->Instance->CCR2 = action_struct->pulse_width; break;
		case	TIM_CHANNEL_3	:	timer->Instance->CCR3 = action_struct->pulse_width; break;
		case	TIM_CHANNEL_4	:	timer->Instance->CCR4 = action_struct->pulse_width; break;
		}
		break;
	case	PWM_EA_SET_PWM_DIRECTION:
		if ( action_struct->pwm_direction )
			HAL_GPIO_WritePin(Pwm_Control_Drv->enable_port[0],Pwm_Control_Drv->enable_bit[0],GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(Pwm_Control_Drv->enable_port[0],Pwm_Control_Drv->enable_bit[0],GPIO_PIN_RESET);
		break;
	default:
		return 1;
	}
	return 0;
}

extern	DriverStruct_t	Pwm_Control_Drv_ArduinoShield;

uint32_t pwm_control_deinit(uint8_t handle)
{
	return driver_unregister(&Pwm_Control_Drv_ArduinoShield);
}

static uint32_t pwm_control_init(uint8_t handle)
{
	return 0;
}

DriverStruct_t	Pwm_Control_Drv_ArduinoShield =
{
	.init = pwm_control_init,
	.deinit = pwm_control_deinit,
	.start = pwm_control_start,
	.stop = pwm_control_stop,
	.extended_action = pwm_control_extended_actions,
	.get_status = pwm_control_get_status,
	.get_values = pwm_control_get_values,
	.set_values = pwm_control_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "pwm_control_ArduinoShield",
};

uint32_t pwm_control_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&Pwm_Control_Drv_ArduinoShield,sizeof(Pwm_Control_Drv_ArduinoShield));
	return 0;
}

#endif // #ifdef A_HAS_MOTOR_CNTRL

