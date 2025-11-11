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

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

#include "pwm.h"
#include <string.h>

ITCM_AREA_CODE uint32_t pwm_start(Pwm_Control_TypeDef *pwm_drv)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;

	switch(pwm_drv->pwm_channel)
	{
	case	TIM_CHANNEL_1	:	timer->Instance->CCR1 = pwm_drv->pulse_width[0]; break;
	case	TIM_CHANNEL_2	:	timer->Instance->CCR2 = pwm_drv->pulse_width[1]; break;
	case	TIM_CHANNEL_3	:	timer->Instance->CCR3 = pwm_drv->pulse_width[2]; break;
	case	TIM_CHANNEL_4	:	timer->Instance->CCR4 = pwm_drv->pulse_width[3]; break;
	case	TIM_CHANNEL_5	:	timer->Instance->CCR5 = pwm_drv->pulse_width[4]; break;
	case	TIM_CHANNEL_6	:	timer->Instance->CCR6 = pwm_drv->pulse_width[5]; break;
	default : return 1;
	}
	if ( HAL_TIM_PWM_Start(timer,pwm_drv->pwm_channel) == 0 )
		pwm_drv->status |= PWM_CHANNEL_RUNNING;
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_stop(Pwm_Control_TypeDef *pwm_drv)
{
	if ( HAL_TIM_PWM_Stop(pwm_drv->timer,pwm_drv->pwm_channel) == 0 )
		pwm_drv->status &= ~PWM_CHANNEL_RUNNING;
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_get_status(Pwm_Control_TypeDef *pwm_drv)
{
	return pwm_drv->status;
}

ITCM_AREA_CODE uint32_t pwm_set_period(Pwm_Control_TypeDef *pwm_drv,uint32_t period)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	pwm_drv->period = timer->Instance->ARR = period;
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_set_prescaler(Pwm_Control_TypeDef *pwm_drv,uint32_t prescaler)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	pwm_drv->prescaler = timer->Instance->PSC = prescaler;
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_set_width(Pwm_Control_TypeDef *pwm_drv,uint32_t pulse_width)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	switch(pwm_drv->pwm_channel)
	{
	case	TIM_CHANNEL_1	:	pwm_drv->pulse_width[0] = timer->Instance->CCR1 = pulse_width; break;
	case	TIM_CHANNEL_2	:	pwm_drv->pulse_width[1] = timer->Instance->CCR2 = pulse_width; break;
	case	TIM_CHANNEL_3	:	pwm_drv->pulse_width[2] = timer->Instance->CCR3 = pulse_width; break;
	case	TIM_CHANNEL_4	:	pwm_drv->pulse_width[3] = timer->Instance->CCR4 = pulse_width; break;
	case	TIM_CHANNEL_5	:	pwm_drv->pulse_width[4] = timer->Instance->CCR5 = pulse_width; break;
	case	TIM_CHANNEL_6	:	pwm_drv->pulse_width[5] = timer->Instance->CCR6 = pulse_width; break;
	default : return 1;
	}
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_set_direction(Pwm_Control_TypeDef *pwm_drv,uint8_t pwm_direction)
{
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

ITCM_AREA_CODE uint32_t pwm_init(Pwm_Control_TypeDef *pwm_drv)
{
	return 0;
}

ITCM_AREA_CODE uint32_t	pwm_register(Pwm_Control_TypeDef *pwm_drv)
{
TIMER_DriverStruct_t *eptr, *pre_eptr;
TIM_HandleTypeDef	*timer = pwm_drv->timer;

	if ( pwm_drv->timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( pwm_drv->period  == 0 )
		return DRIVER_REQUEST_FAILED;
	if ( timer_drv_ptr == NULL)
	{
		timer_drv_ptr = (TIMER_DriverStruct_t *)pwm_drv;
		pwm_drv->next_timer = NULL;
	}
	else
	{
		eptr = pre_eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
		{
			pre_eptr = eptr;
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		}
		pre_eptr->next_timer = (uint32_t *)pwm_drv;
		pwm_drv->next_timer = NULL;
	}
	pwm_drv->process = get_current_process();
	timer->Instance->ARR = pwm_drv->period;

	return 0;
}

#endif // #ifdef A_OS_TIMERS_ENABLED

