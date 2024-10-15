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

static Pwm_Control_Drv_TypeDef	Pwm_Control_Drv;

static uint32_t pwm_control_start(void)
{
	Pwm_Control_Drv.status |= PWM_CHANNEL_RUNNING;
	HAL_TIM_PWM_Start(Pwm_Control_Drv.timer,Pwm_Control_Drv.channel);
	return 0;
}

static uint32_t pwm_control_stop(void)
{
	Pwm_Control_Drv.status &= ~PWM_CHANNEL_RUNNING;
	HAL_TIM_PWM_Stop(Pwm_Control_Drv.timer,Pwm_Control_Drv.channel);
	return 0;
}

static uint32_t pwm_control_get_status(void)
{
	return 0;
}

static uint32_t pwm_control_get_values(uint8_t *values)
{
	return 0;
}

static uint32_t pwm_control_select_channel(uint8_t channel)
{
	if ( channel >= A_HAS_MOTOR_CNTRL_NUMBER )
		return 1;
	return 0;
}

static uint32_t pwm_control_set_values(uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t pwm_control_extended_actions(uint8_t action,uint32_t action_parameter,uint32_t extension_parameter)
{
	switch ( action )
	{
	case	PWM_EA_SET_PWM_PERIOD:
		switch(Pwm_Control_Drv.channel)
		{
		case	TIM_CHANNEL_1	:	Pwm_Control_Drv.timer->Instance->CCR1 = action_parameter; break;
		case	TIM_CHANNEL_2	:	Pwm_Control_Drv.timer->Instance->CCR2 = action_parameter; break;
		case	TIM_CHANNEL_3	:	Pwm_Control_Drv.timer->Instance->CCR3 = action_parameter; break;
		case	TIM_CHANNEL_4	:	Pwm_Control_Drv.timer->Instance->CCR4 = action_parameter; break;
		}
		break;
	case	PWM_EA_SET_PWM_DIRECTION:
		break;
	default:
		return 1;
	}
	return 0;
}

extern	DriversDefsActuators_t	Pwm_Control_Drv_ArduinoShield;

uint32_t pwm_control_deinit(void)
{
	return actuators_driver_unregister(&Pwm_Control_Drv_ArduinoShield);
}

static uint32_t pwm_control_init(void)
{
	Pwm_Control_Drv.timer = (TIM_HandleTypeDef *)Pwm_Control_Drv_ArduinoShield.peripheral;
	Pwm_Control_Drv.channel = Pwm_Control_Drv_ArduinoShield.peripheral_channel;
	Pwm_Control_Drv.status |= PWM_CHANNEL_INITIALIZED;
	return 0;
}

DriversDefsActuators_t	Pwm_Control_Drv_ArduinoShield =
{
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.peripheral = (uint32_t *)&MOTOR_CNTRL_PWM_A,
	.peripheral_channel = MOTOR_CNTRL_CHANNEL_A,
	.init = pwm_control_init,
	.deinit = pwm_control_deinit,
	.start = pwm_control_start,
	.stop = pwm_control_stop,
	.select_channel = pwm_control_select_channel,
	.extended_action = pwm_control_extended_actions,
	.get_status = pwm_control_get_status,
	.get_values = pwm_control_get_values,
	.set_values = pwm_control_set_values,
	.driver_name = "pwm_control_ArduinoShield",
};


DriversDefsActuators_t *pwm_control_set_drv_struct(void)
{
	return &Pwm_Control_Drv_ArduinoShield;
}

#endif // #ifdef A_HAS_MOTOR_CNTRL

