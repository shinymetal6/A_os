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

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

#include "servo.h"
#include <string.h>

ITCM_AREA_CODE uint32_t servo_start(SERVO_Control_DriverStruct_t *servo_drv)
{
	HAL_TIM_PWM_Start(servo_drv->timer,servo_drv->servo_channel);
	__HAL_TIM_DISABLE(servo_drv->timer);
	return 0;
}

ITCM_AREA_CODE uint32_t servo_stop(SERVO_Control_DriverStruct_t *servo_drv)
{
	return 0;
}

ITCM_AREA_CODE uint32_t servo_get_status(SERVO_Control_DriverStruct_t *servo_drv)
{
	return 0;
}

ITCM_AREA_CODE uint32_t servo_set_prescaler(SERVO_Control_DriverStruct_t *servo_drv,uint32_t prescaler)
{
	return 0;
}

ITCM_AREA_CODE uint32_t servo_set_position(SERVO_Control_DriverStruct_t *servo_drv,uint8_t servo_position,uint8_t servo_pulses)
{
uint16_t				span_time;
uint16_t				servo_pulse_len;

	// position is expressed in percentage : 0 .. 100
	if ( servo_position > 100 )
		return DRIVER_STATUS_FAILED;

	__HAL_TIM_DISABLE(servo_drv->timer);
	span_time = servo_drv->max_time - servo_drv->min_time;
	servo_pulse_len = servo_drv->min_time + ((span_time / 100 ) * servo_position);
	servo_drv->timer->Instance->CNT = 0;
	servo_drv->timer->Instance->ARR = servo_drv->cycle_time;
	servo_drv->timer->Instance->CCR1 = servo_drv->cycle_time - servo_pulse_len;
	servo_drv->timer->Instance->RCR = servo_drv->repetition;
    __HAL_TIM_ENABLE(servo_drv->timer);
	return 0;
}

ITCM_AREA_CODE uint32_t	servo_register(SERVO_Control_DriverStruct_t *servo_drv)
{
TIMER_DriverStruct_t *eptr;

	if ( servo_drv->timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( timer_drv_ptr == NULL)
	{
		timer_drv_ptr = (TIMER_DriverStruct_t *)servo_drv;
		servo_drv->next_timer = NULL;
	}
	else
	{
		eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		eptr->next_timer = (uint32_t *)servo_drv;
		servo_drv->next_timer = NULL;
	}
	servo_drv->process = get_current_process();
	return 0;
}

#endif // #ifdef A_OS_TIMERS_ENABLED
