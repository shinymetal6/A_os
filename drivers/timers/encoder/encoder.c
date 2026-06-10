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
 * encoder.c
 *
 *  Created on: Oct 6, 2025
 *      Author: fil
 */
#include "main.h"

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED
#include "encoder.h"
#include <string.h>

ITCM_AREA_CODE uint32_t	encoder_register(Encoder_DriverStruct_t *encoder_drv)
{
TIMER_DriverStruct_t *eptr, *pre_eptr;

	if ( encoder_drv->timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( timer_drv_ptr == NULL)
	{
		timer_drv_ptr = (TIMER_DriverStruct_t *)encoder_drv;
		encoder_drv->next_timer = NULL;
	}
	else
	{
		eptr = pre_eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
		{
			pre_eptr = eptr;
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		}
		pre_eptr->next_timer = (uint32_t *)encoder_drv;
		encoder_drv->next_timer = NULL;
	}
	encoder_drv->timer_type = TIM_TYPE_ENCODER;
	encoder_drv->process =  get_current_process();
	HAL_TIM_Encoder_Start_IT(encoder_drv->timer, TIM_CHANNEL_ALL);
	return 0;
}

#endif // #ifdef A_OS_TIMERS_ENABLED


