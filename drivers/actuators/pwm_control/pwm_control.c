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

static Pwm_Control_Drv_TypeDef	Pwm_Control_Drv;
extern	DriversDefs_t	*DriversDefs[MAX_DRIVERS];

static uint32_t pwm_control_start(uint8_t handle)
{
	if ( HAL_TIM_PWM_Start((TIM_HandleTypeDef *)DriversDefs[handle]->peripheral,DriversDefs[handle]->peripheral_channel) == 0 )
		DriversDefs[handle]->status |= DRIVER_STATUS_RUNNING;
	return 0;
}

static uint32_t pwm_control_stop(uint8_t handle)
{
	if ( HAL_TIM_PWM_Stop((TIM_HandleTypeDef *)DriversDefs[handle]->peripheral,DriversDefs[handle]->peripheral_channel) == 0 )
		DriversDefs[handle]->status &= ~DRIVER_STATUS_RUNNING;
	return 0;
}

static uint32_t pwm_control_get_status(uint8_t handle)
{
	return DriversDefs[handle]->status;
}

static uint32_t pwm_control_get_values(uint8_t handle,uint8_t *values)
{
	return 0;
}

static uint32_t pwm_control_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t pwm_control_extended_actions(uint8_t handle,uint8_t action,uint32_t action_parameter,uint32_t extension_parameter)
{
TIM_HandleTypeDef *timer = (TIM_HandleTypeDef *)DriversDefs[handle]->peripheral;
	switch ( action )
	{
	case	PWM_EA_SET_PWM_PRESCALER:
		timer->Instance->PSC = Pwm_Control_Drv.prescaler = action_parameter;
		break;
	case	PWM_EA_SET_PWM_PERIOD:
		switch(DriversDefs[handle]->peripheral_channel)
		{
		case	TIM_CHANNEL_1	:	timer->Instance->CCR1 = action_parameter; Pwm_Control_Drv.pulse_width = action_parameter;break;
		case	TIM_CHANNEL_2	:	timer->Instance->CCR2 = action_parameter; Pwm_Control_Drv.pulse_width = action_parameter;break;
		case	TIM_CHANNEL_3	:	timer->Instance->CCR3 = action_parameter; Pwm_Control_Drv.pulse_width = action_parameter;break;
		case	TIM_CHANNEL_4	:	timer->Instance->CCR4 = action_parameter; Pwm_Control_Drv.pulse_width = action_parameter;break;
		}
		break;
	case	PWM_EA_SET_PWM_DIRECTION:
		if ( action_parameter )
			HAL_GPIO_WritePin((GPIO_TypeDef *)DriversDefs[handle]->gpio_port[0],DriversDefs[handle]->gpio_bit[0],GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin((GPIO_TypeDef *)DriversDefs[handle]->gpio_port[0],DriversDefs[handle]->gpio_bit[0],GPIO_PIN_RESET);
		break;
	default:
		return 1;
	}
	return 0;
}

extern	DriversDefs_t	Pwm_Control_Drv_ArduinoShield;

uint32_t pwm_control_deinit(uint8_t handle)
{
	return driver_unregister(&Pwm_Control_Drv_ArduinoShield);
}

static uint32_t pwm_control_init(uint8_t handle)
{
	return 0;
}

DriversDefs_t	Pwm_Control_Drv_ArduinoShield =
{
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.peripheral = (uint32_t *)&MOTOR_CNTRL_PWM_A,
	.peripheral_channel = MOTOR_CNTRL_CHANNEL_A,
	.init = pwm_control_init,
	.deinit = pwm_control_deinit,
	.start = pwm_control_start,
	.stop = pwm_control_stop,
	.extended_action = pwm_control_extended_actions,
	.get_status = pwm_control_get_status,
	.get_values = pwm_control_get_values,
	.set_values = pwm_control_set_values,
	.driver_name = "pwm_control_ArduinoShield",
};

uint32_t pwm_control_get_drv_struct(DriversDefs_t *new_struct,uint8_t peripheral_index)
{
	memcpy(new_struct,&Pwm_Control_Drv_ArduinoShield,sizeof(Pwm_Control_Drv_ArduinoShield));
	new_struct->peripheral_index = peripheral_index;
	return 0;
}

#endif // #ifdef A_HAS_MOTOR_CNTRL

