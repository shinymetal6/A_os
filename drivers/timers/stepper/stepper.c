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
extern	TIM_HandleTypeDef htim16;

ITCM_AREA_CODE uint32_t stepper_start(Stepper_Control_TypeDef *stepper_drv,uint32_t stepper_channel,uint8_t	number_of_steps)
{
TIM_HandleTypeDef	*timer = stepper_drv->timer;
	timer->Instance->RCR = stepper_drv->work_number_of_steps = stepper_drv->number_of_steps = number_of_steps - 1;
	if ( HAL_TIM_PWM_Start(timer,stepper_channel) == 0 )
	{
		__HAL_TIM_ENABLE_IT(timer, TIM_IT_UPDATE);
		stepper_drv->status |= STEPPER_CHANNEL_STARTED;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t stepper_stop(Stepper_Control_TypeDef *stepper_drv,uint32_t stepper_channel)
{
TIM_HandleTypeDef	*timer = stepper_drv->timer;
	if ( HAL_TIM_PWM_Stop(timer,stepper_channel) == 0 )
	{
        __HAL_TIM_DISABLE_IT(stepper_drv->timer, TIM_IT_UPDATE);
		stepper_drv->status &= ~STEPPER_CHANNEL_STARTED;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t stepper_set_direction(Stepper_Control_TypeDef *stepper_drv,uint8_t direction)
{
	if ( stepper_drv->dir_port != NULL)
	{
		if ( stepper_drv->dir_bit )
			HAL_GPIO_WritePin(stepper_drv->dir_port, stepper_drv->dir_bit, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(stepper_drv->dir_port, stepper_drv->dir_bit, GPIO_PIN_RESET);
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t stepper_init(Stepper_Control_TypeDef *stepper_drv)
{
TIM_HandleTypeDef	*timer = stepper_drv->timer;
	if ( stepper_drv->prescaler )
		timer->Instance->PSC = stepper_drv->prescaler;
	if ( stepper_drv->period )
		timer->Instance->ARR = stepper_drv->period;
	stepper_drv->status |= STEPPER_CHANNEL_INITIALIZED;
	return 0;
}

ITCM_AREA_CODE uint32_t	stepper_register(Stepper_Control_TypeDef *stepper_drv)
{
TIMER_DriverStruct_t *eptr, *pre_eptr;

	if ( stepper_drv->timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( stepper_drv->dir_port == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( timer_drv_ptr == NULL)
	{
		timer_drv_ptr = (TIMER_DriverStruct_t *)stepper_drv;
		stepper_drv->next_timer = NULL;
	}
	else
	{
		eptr = pre_eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
		{
			pre_eptr = eptr;
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		}
		pre_eptr->next_timer = (uint32_t *)stepper_drv;
		stepper_drv->next_timer = NULL;
	}
	stepper_drv->process = get_current_process();
	stepper_drv->timer_type = TIM_TYPE_PWM;

	return 0;
}


#endif // #ifdef A_OS_TIMERS_ENABLED
