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
 * periodic_timer.c
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#include "main.h"

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_TIMERS_ENABLED

#include "periodic_timer.h"
#include <string.h>

ITCM_AREA_CODE uint32_t periodic_timer_start(PERIODIC_Timer_Drv_TypeDef *periodic_timer_drv)
{
	if ( HAL_TIM_Base_Start_IT(periodic_timer_drv->timer) == 0 )
	{
		periodic_timer_drv->status |= PERIODIC_TIMER_RUNNING;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t	periodic_timer_register(PERIODIC_Timer_Drv_TypeDef *periodic_timer_drv)
{
TIMER_DriverStruct_t *eptr, *pre_eptr;

	if ( periodic_timer_drv->timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( periodic_timer_drv->User_Callback == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( timer_drv_ptr == NULL)
	{
		timer_drv_ptr = (TIMER_DriverStruct_t *)periodic_timer_drv;
		periodic_timer_drv->next_timer = NULL;
	}
	else
	{
		eptr = pre_eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
		{
			pre_eptr = eptr;
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		}
		pre_eptr->next_timer = (uint32_t *)periodic_timer_drv;
		periodic_timer_drv->next_timer = NULL;
	}
	periodic_timer_drv->process = get_current_process();
	periodic_timer_drv->timer_type = TIM_TYPE_PERIODIC;
	periodic_timer_drv->status = PERIODIC_TIMER_INITIALIZED;
	return 0;
}



#endif // #ifdef A_OS_TIMERS_ENABLED

