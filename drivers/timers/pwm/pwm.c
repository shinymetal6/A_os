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

ITCM_AREA_CODE uint32_t pwm_start(Pwm_Control_DriverStruct_t *pwm_drv,uint32_t pwm_channel)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
uint8_t				ch_index;
	switch(pwm_channel)
	{
	case	TIM_CHANNEL_1	:	timer->Instance->CCR1 = pwm_drv->pulse_width[0]; ch_index = PWM_CHANNEL_CH1;break;
	case	TIM_CHANNEL_2	:	timer->Instance->CCR2 = pwm_drv->pulse_width[1]; ch_index = PWM_CHANNEL_CH2;break;
	case	TIM_CHANNEL_3	:	timer->Instance->CCR3 = pwm_drv->pulse_width[2]; ch_index = PWM_CHANNEL_CH3;break;
	case	TIM_CHANNEL_4	:	timer->Instance->CCR4 = pwm_drv->pulse_width[3]; ch_index = PWM_CHANNEL_CH4;break;
	case	TIM_CHANNEL_5	:	timer->Instance->CCR5 = pwm_drv->pulse_width[4]; ch_index = PWM_CHANNEL_CH5;break;
	case	TIM_CHANNEL_6	:	timer->Instance->CCR6 = pwm_drv->pulse_width[5]; ch_index = PWM_CHANNEL_CH6;break;
	default : return 1;
	}
	if ( HAL_TIM_PWM_Start(timer,pwm_channel) == 0 )
	{
		pwm_drv->status |= ch_index;
		pwm_drv->status |= PWM_STARTED;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t pwm_start_all_enabled(Pwm_Control_DriverStruct_t *pwm_drv)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	if ( pwm_drv->status & PWM_CHANNEL_CH1)
	{
		if ( HAL_TIM_PWM_Start(timer,TIM_CHANNEL_1) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH2)
	{
		if ( HAL_TIM_PWM_Start(timer,TIM_CHANNEL_2) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH3)
	{
		if ( HAL_TIM_PWM_Start(timer,TIM_CHANNEL_3) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH4)
	{
		if ( HAL_TIM_PWM_Start(timer,TIM_CHANNEL_4) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH5)
	{
		if ( HAL_TIM_PWM_Start(timer,TIM_CHANNEL_5) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH6)
	{
		if ( HAL_TIM_PWM_Start(timer,TIM_CHANNEL_6) )
			return 1;
	}
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_stop(Pwm_Control_DriverStruct_t *pwm_drv,uint32_t pwm_channel)
{
uint8_t				ch_index;
	switch(pwm_channel)
	{
	case	TIM_CHANNEL_1	:	ch_index = PWM_CHANNEL_CH1;break;
	case	TIM_CHANNEL_2	:	ch_index = PWM_CHANNEL_CH2;break;
	case	TIM_CHANNEL_3	:	ch_index = PWM_CHANNEL_CH3;break;
	case	TIM_CHANNEL_4	:	ch_index = PWM_CHANNEL_CH4;break;
	case	TIM_CHANNEL_5	:	ch_index = PWM_CHANNEL_CH5;break;
	case	TIM_CHANNEL_6	:	ch_index = PWM_CHANNEL_CH6;break;
	default : return 1;
	}
	if ( HAL_TIM_PWM_Stop(pwm_drv->timer,pwm_channel) == 0 )
		pwm_drv->status &= ~ch_index;
	if (( pwm_drv->status & ~(PWM_CHANNEL_INITIALIZED|PWM_STARTED)) == 0 )
		pwm_drv->status &= ~(PWM_CHANNEL_INITIALIZED|PWM_STARTED);
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_stop_all_enabled(Pwm_Control_DriverStruct_t *pwm_drv)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	if ( pwm_drv->status & PWM_CHANNEL_CH1)
	{
		pwm_drv->status |= PWM_CHANNEL_CH1;
		if ( HAL_TIM_PWM_Stop(timer,TIM_CHANNEL_1) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH2)
	{
		pwm_drv->status |= PWM_CHANNEL_CH2;
		if ( HAL_TIM_PWM_Stop(timer,TIM_CHANNEL_2) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH3)
	{
		pwm_drv->status |= PWM_CHANNEL_CH3;
		if ( HAL_TIM_PWM_Stop(timer,TIM_CHANNEL_3) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH4)
	{
		pwm_drv->status |= PWM_CHANNEL_CH4;
		if ( HAL_TIM_PWM_Stop(timer,TIM_CHANNEL_4) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH5)
	{
		pwm_drv->status |= PWM_CHANNEL_CH5;
		if ( HAL_TIM_PWM_Stop(timer,TIM_CHANNEL_5) )
			return 1;
	}
	if ( pwm_drv->status & PWM_CHANNEL_CH6)
	{
		pwm_drv->status |= PWM_CHANNEL_CH6;
		if ( HAL_TIM_PWM_Stop(timer,TIM_CHANNEL_6) )
			return 1;
	}
	if (( pwm_drv->status & ~(PWM_CHANNEL_INITIALIZED|PWM_STARTED)) == 0 )
		pwm_drv->status &= ~(PWM_CHANNEL_INITIALIZED);
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_get_status(Pwm_Control_DriverStruct_t *pwm_drv)
{
	return pwm_drv->status;
}

ITCM_AREA_CODE uint32_t pwm_set_period(Pwm_Control_DriverStruct_t *pwm_drv,uint32_t period)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	pwm_drv->period = timer->Instance->ARR = period;
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_set_prescaler(Pwm_Control_DriverStruct_t *pwm_drv,uint32_t prescaler)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	pwm_drv->prescaler = timer->Instance->PSC = prescaler;
	return 0;
}

ITCM_AREA_CODE uint32_t pwm_set_width(Pwm_Control_DriverStruct_t *pwm_drv,uint32_t pulse_width,uint32_t pwm_channel)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	switch(pwm_channel)
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

ITCM_AREA_CODE uint32_t pwm_init(Pwm_Control_DriverStruct_t *pwm_drv)
{
TIM_HandleTypeDef	*timer = pwm_drv->timer;
	if ( pwm_drv->pulse_width[0])
	{
		timer->Instance->CCR1 = pwm_drv->pulse_width[0];
		pwm_drv->status |= PWM_CHANNEL_CH1;
	}
	if ( pwm_drv->pulse_width[1])
	{
		timer->Instance->CCR2 = pwm_drv->pulse_width[1];
		pwm_drv->status |= PWM_CHANNEL_CH2;
	}
	if ( pwm_drv->pulse_width[2])
	{
		timer->Instance->CCR3 = pwm_drv->pulse_width[2];
		pwm_drv->status |= PWM_CHANNEL_CH3;
	}
	if ( pwm_drv->pulse_width[3])
	{
		timer->Instance->CCR4 = pwm_drv->pulse_width[3];
		pwm_drv->status |= PWM_CHANNEL_CH4;
	}
	if ( pwm_drv->pulse_width[4])
	{
		timer->Instance->CCR5 = pwm_drv->pulse_width[4];
		pwm_drv->status |= PWM_CHANNEL_CH5;
	}
	if ( pwm_drv->pulse_width[5])
	{
		timer->Instance->CCR6 = pwm_drv->pulse_width[5];
		pwm_drv->status |= PWM_CHANNEL_CH6;
	}
	if ( pwm_drv->prescaler )
		timer->Instance->PSC = pwm_drv->prescaler;
	timer->Instance->ARR = pwm_drv->period;
	pwm_drv->status |= PWM_CHANNEL_INITIALIZED;
	return 0;
}

ITCM_AREA_CODE uint32_t	pwm_register(Pwm_Control_DriverStruct_t *pwm_drv)
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

