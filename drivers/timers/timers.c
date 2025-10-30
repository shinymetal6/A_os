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
 * timers.c
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef A_OS_TIMERS_ENABLED


TIMER_DriverStruct_t	*timer_drv_ptr;

ITCM_AREA_CODE TIMER_DriverStruct_t *get_ptr_from_workers(TIM_HandleTypeDef 	*timer)
{
TIMER_DriverStruct_t *eptr, *pre_eptr;

	eptr = pre_eptr = timer_drv_ptr;
	while(eptr != NULL)
	{
		if ( eptr->timer == timer )
			return eptr;
		pre_eptr = eptr;
		if ( eptr->next_timer == NULL )
			return NULL;
		eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
	}
	return NULL;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
uint32_t j;
TIMER_DriverStruct_t *tim_ic = get_ptr_from_workers(htim);

	if ( tim_ic == NULL )
		return;
	if ( tim_ic->timer_type == TIM_TYPE_DHT11 )
	{
		for(j=0;j<MAX_DHT11_DEVICES;j++)
			tim_ic->status = DHTXX_AM230X_ACQDONE;
	}
	if ( tim_ic->timer_type == TIM_TYPE_ENCODER )
	{
		if ( tim_ic->timer == htim)
		{
			Encoder_Drv_TypeDef	*encoder_driver_data = (Encoder_Drv_TypeDef *)tim_ic;
			encoder_driver_data->encoder_value = (htim->Instance->CNT>>2);
			if ( encoder_driver_data->encoder_value != encoder_driver_data->encoder_last_value )
			{
				encoder_driver_data->status &= ~(ENCODER_UP | ENCODER_DOWN);

				if ( encoder_driver_data->encoder_value > encoder_driver_data->encoder_last_value)
					encoder_driver_data->status |= ENCODER_UP;
				else
					encoder_driver_data->status |= ENCODER_DOWN;
				encoder_driver_data->status |= ENCODER_READY;
				encoder_driver_data->encoder_last_value = encoder_driver_data->encoder_value;
				if ( encoder_driver_data->irq_encoder_callback != NULL )
					encoder_driver_data->irq_encoder_callback(encoder_driver_data->encoder_value);
				if ( encoder_driver_data->wakeup_id )
					activate_process(encoder_driver_data->process,encoder_driver_data->wakeup_id,0x01);
			}
		}
	}
}


#endif

