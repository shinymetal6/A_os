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
 * hw_timers.c
 *
 *  Created on: Mar 4, 2024
 *      Author: fil
 */

#include "main.h"
#include "../system_default.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../kernel_opt.h"
#include "hw_timers.h"

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;
#ifdef A_HAS_TIMERS
extern	HW_Timers_t	HW_Timers[A_MAX_TIMERS];

ITCM_AREA_CODE void timer_callback(TIM_HandleTypeDef *htim)
{
uint32_t	i, flag_field = 0;
uint8_t		current_owner , last_owner=0;
	for(i=0;i<A_MAX_TIMERS;i++)
	{
		if ( HW_Timers[i].hwtimer_handle == htim )
		{
			current_owner = HW_Timers[i].hwtimer_owner;
			// check if it's first time this timer is scanned
			if ( current_owner == last_owner )
				flag_field |= (1 << i);	// already set, so do or
			else
				flag_field = (1 << i);	// never set, so set for the first time
			activate_process(HW_Timers[i].hwtimer_owner,HW_TIM,flag_field );	// sends timer number as flag
			last_owner = HW_Timers[i].hwtimer_owner;
		}
	}
}

void A_hw_timers_start_pwm(uint8_t hwtimer_id, uint32_t Channel)
{
	HAL_TIM_PWM_Start(HW_Timers[hwtimer_id].hwtimer_handle, Channel);
	HAL_TIMEx_PWMN_Start_IT(HW_Timers[hwtimer_id].hwtimer_handle, Channel);

}

void A_hw_timers_init(void)
{
#ifdef	A_HAS_TIMER1
	HW_Timers[0].hwtimer_handle = &htim1;
	HW_Timers[0].hwtimer_index  = HW_TIM;
	HW_Timers[0].hwtimer_owner  = 0;
	HW_Timers[0].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER2
	HW_Timers[1].hwtimer_handle = &htim2;
	HW_Timers[1].hwtimer_index  = HW_TIM;
	HW_Timers[1].hwtimer_owner  = 0;
	HW_Timers[1].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER3
	HW_Timers[2].hwtimer_handle = &htim3;
	HW_Timers[2].hwtimer_index  = HW_TIM;
	HW_Timers[2].hwtimer_owner  = 0;
	HW_Timers[2].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER4
	HW_Timers[3].hwtimer_handle = &htim4;
	HW_Timers[3].hwtimer_index  = HW_TIM;
	HW_Timers[3].hwtimer_owner  = 0;
	HW_Timers[3].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER5
	HW_Timers[4].hwtimer_handle = &htim5;
	HW_Timers[4].hwtimer_index  = HW_TIM;
	HW_Timers[4].hwtimer_owner  = 0;
	HW_Timers[4].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER6
	HW_Timers[5].hwtimer_handle = &htim6;
	HW_Timers[5].hwtimer_index  = HW_TIM;
	HW_Timers[5].hwtimer_owner  = 0;
	HW_Timers[5].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER7
	HW_Timers[6].hwtimer_handle = &htim7;
	HW_Timers[6].hwtimer_index  = HW_TIM;
	HW_Timers[6].hwtimer_owner  = 0;
	HW_Timers[6].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER8
	HW_Timers[7].hwtimer_handle = &htim8;
	HW_Timers[7].hwtimer_index  = HW_TIM;
	HW_Timers[7].hwtimer_owner  = 0;
	HW_Timers[7].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER9
	HW_Timers[8].hwtimer_handle = &htim9;
	HW_Timers[8].hwtimer_index  = HW_TIM;
	HW_Timers[8].hwtimer_owner  = 0;
	HW_Timers[8].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER10
	HW_Timers[9].hwtimer_handle = &htim10;
	HW_Timers[9].hwtimer_index  = HW_TIM;
	HW_Timers[9].hwtimer_owner  = 0;
	HW_Timers[9].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER11
	HW_Timers[10].hwtimer_handle= &htim11;
	HW_Timers[10].hwtimer_index = HW_TIM;
	HW_Timers[10].hwtimer_owner = 0;
	HW_Timers[10].hwtimer_flags = 0;
#endif
#ifdef	A_HAS_TIMER12
	HW_Timers[11].hwtimer_handle= &htim12;
	HW_Timers[11].hwtimer_index = HW_TIM;
	HW_Timers[11].hwtimer_owner = 0;
	HW_Timers[11].hwtimer_flags = 0;
#endif
#ifdef	A_HAS_TIMER13
	HW_Timers[12].hwtimer_handle = &htim13;
	HW_Timers[12].hwtimer_index  = HW_TIM;
	HW_Timers[12].hwtimer_owner  = 0;
	HW_Timers[12].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER14
	HW_Timers[13].hwtimer_handle = &htim14;
	HW_Timers[13].hwtimer_index  = HW_TIM;
	HW_Timers[13].hwtimer_owner  = 0;
	HW_Timers[13].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER15
	HW_Timers[14].hwtimer_handle = &htim15;
	HW_Timers[14].hwtimer_index  = HW_TIM;
	HW_Timers[14].hwtimer_owner  = 0;
	HW_Timers[14].hwtimer_flags 	= 0;
#endif
#ifdef	A_HAS_TIMER16
	HW_Timers[15].hwtimer_handle= &htim16;
	HW_Timers[15].hwtimer_index = HW_TIM;
	HW_Timers[15].hwtimer_owner = 0;
	HW_Timers[15].hwtimer_flags	= 0;
#endif
#ifdef	A_HAS_TIMER17
	HW_Timers[16].hwtimer_handle= &htim17;
	HW_Timers[16].hwtimer_index = HW_TIM;
	HW_Timers[16].hwtimer_owner = 0;
	HW_Timers[16].hwtimer_flags	= 0;
#endif
}
#else	//#ifdef A_HAS_TIMERS
void A_hw_timers_init(void)
{

}
#endif	//#ifdef A_HAS_TIMERS
