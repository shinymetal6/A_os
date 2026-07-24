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
 * stepper.c
 *
 *  Created on: Jun 9, 2026
 *      Author: fil
 */

#include "main.h"

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

#include "stepper.h"
#include <string.h>

void stepper_internal_callback(Stepper_Control_DriverStruct_t *stepper_drv)
{
TIM_HandleTypeDef	*timer = stepper_drv->timer;

	if ( stepper_drv->number_of_steps == 0 )
	{
		HAL_TIM_PWM_Stop(stepper_drv->timer, stepper_drv->timer_channel);
		__HAL_TIM_DISABLE_IT(stepper_drv->timer, TIM_IT_UPDATE);
		stepper_drv->status &= ~STEPPER_CHANNEL_STARTED;
		if ( stepper_drv->stepper_callback != NULL )
			stepper_drv->stepper_callback(stepper_drv->stored_number_of_steps);
		HAL_GPIO_WritePin(stepper_drv->enable_port, stepper_drv->enable_bit, GPIO_PIN_SET);
		return;
	}

	if ( stepper_drv->number_of_steps > STEPPER_RCR_MAXVAL )
		stepper_drv->number_of_steps -= STEPPER_RCR_MAXVAL;
	else
	{
		if ( stepper_drv->number_of_steps > 0 )
			timer->Instance->RCR = stepper_drv->number_of_steps;
		else
			timer->Instance->RCR = STEPPER_RCR_MAXVAL;
		stepper_drv->number_of_steps = 0;
	}
}

ITCM_AREA_CODE uint32_t stepper_start(Stepper_Control_DriverStruct_t *stepper_drv,uint32_t stepper_channel,uint8_t	number_of_rotation,uint8_t direction)
{
TIM_HandleTypeDef	*timer = stepper_drv->timer;

	if ( ( stepper_drv->status & (STEPPER_CHANNEL_READY | STEPPER_CHANNEL_INITIALIZED )) != (STEPPER_CHANNEL_READY | STEPPER_CHANNEL_INITIALIZED ) )
		return 1;

	stepper_drv->direction = direction;
	if ( stepper_drv->direction )
		HAL_GPIO_WritePin(stepper_drv->dir_port, stepper_drv->dir_bit, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(stepper_drv->dir_port, stepper_drv->dir_bit, GPIO_PIN_RESET);
	stepper_drv->number_of_rotation = number_of_rotation;
	if ( stepper_drv->number_of_rotation )
	{
		stepper_drv->stored_number_of_steps = stepper_drv->number_of_steps = (stepper_drv->number_of_rotation * STEPPER_RCR_MAXVAL);
		if ( stepper_drv->number_of_steps > STEPPER_RCR_MAXVAL )
			timer->Instance->RCR = STEPPER_RCR_MAXVAL;
		else
			timer->Instance->RCR = stepper_drv->number_of_steps;
	}
	else
		timer->Instance->RCR = 0;
	if ( HAL_TIM_PWM_Start(timer,stepper_channel) )
		return 1;
	HAL_GPIO_WritePin(stepper_drv->enable_port, stepper_drv->enable_bit, GPIO_PIN_RESET);
	timer->Instance->EGR = TIM_EVENTSOURCE_UPDATE;

	__HAL_TIM_CLEAR_FLAG(timer, TIM_FLAG_UPDATE);

	__HAL_TIM_ENABLE_IT(timer, TIM_IT_UPDATE);
	stepper_drv->status |= STEPPER_CHANNEL_STARTED;
	return 0;
}

ITCM_AREA_CODE uint32_t stepper_stop(Stepper_Control_DriverStruct_t *stepper_drv,uint32_t stepper_channel)
{
TIM_HandleTypeDef	*timer = stepper_drv->timer;
	if ( HAL_TIM_PWM_Stop(timer,stepper_channel) == 0 )
	{
        __HAL_TIM_DISABLE_IT(stepper_drv->timer, TIM_IT_UPDATE);
		stepper_drv->status &= ~STEPPER_CHANNEL_STARTED;
		HAL_GPIO_WritePin(stepper_drv->enable_port, stepper_drv->enable_bit, GPIO_PIN_SET);

		return 0;
	}
	return 1;
}


ITCM_AREA_CODE uint32_t stepper_set_prescaler(Stepper_Control_DriverStruct_t *stepper_drv,uint32_t prescaler)
{
TIM_HandleTypeDef	*timer = stepper_drv->timer;
	if ( prescaler )
	{
		stepper_drv->prescaler = prescaler;
		timer->Instance->PSC = stepper_drv->prescaler;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t stepper_init(Stepper_Control_DriverStruct_t *stepper_drv)
{
TIM_HandleTypeDef	*timer = stepper_drv->timer;
	if (( stepper_drv->status & STEPPER_CHANNEL_READY ) == STEPPER_CHANNEL_READY)
	{
		if ( stepper_drv->prescaler )
			timer->Instance->PSC = stepper_drv->prescaler;
		if ( stepper_drv->period )
			timer->Instance->ARR = stepper_drv->period;
		stepper_drv->status |= STEPPER_CHANNEL_INITIALIZED;
		HAL_GPIO_WritePin(stepper_drv->enable_port, stepper_drv->enable_bit, GPIO_PIN_SET);

		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t	stepper_register(Stepper_Control_DriverStruct_t *stepper_drv)
{
TIMER_DriverStruct_t *eptr;

	if ( stepper_drv->timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( stepper_drv->tim_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( stepper_drv->dir_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( stepper_drv->enable_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( stepper_drv->steps_per_rotation == 0)
		stepper_drv->steps_per_rotation = STEPPER_DEFAULT_STEP_PER_ROTATION;
	if ( timer_drv_ptr == NULL)
	{
		timer_drv_ptr = (TIMER_DriverStruct_t *)stepper_drv;
		stepper_drv->next_timer = NULL;
	}
	else
	{
		eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		eptr->next_timer = (uint32_t *)stepper_drv;
		stepper_drv->next_timer = NULL;
	}
	stepper_drv->process = get_current_process();
	stepper_drv->timer_type = TIM_TYPE_STEPPER;
	set_gpio_mode(stepper_drv->tim_port,stepper_drv->tim_bit,MODE_AF);
	stepper_drv->status |= STEPPER_CHANNEL_READY;
	return 0;
}


#endif // #ifdef A_OS_TIMERS_ENABLED
