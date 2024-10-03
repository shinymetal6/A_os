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
 * timer.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "system_default.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

extern	PCB_t 		process[MAX_PROCESS];
extern	Asys_t		Asys;

extern	__IO uint32_t uwTick;

#ifdef	A_HAS_UARTS
void 		(*before_check_timers_callback)(void) = HAL_UART_RxTimeoutCheckCallback;
void 		(*after_check_timers_callback)(void)  = NULL;
#else
void 		(*before_check_timers_callback)(void) = NULL;
void 		(*after_check_timers_callback)(void)  = NULL;
#endif

ITCM_AREA_CODE void set_before_check_timers_callback(void (*callback)(void))
{
	before_check_timers_callback = callback;
}

ITCM_AREA_CODE void set_after_check_timers_callback(void (*callback)(void))
{
	after_check_timers_callback = callback;
}

ITCM_AREA_CODE void update_global_tick_count(void)
{
	__disable_irq();
	Asys.g_tick_count++;
	// update the HAL timer, if someone need it
	uwTick++;

	__enable_irq();
}

ITCM_AREA_CODE int32_t A_GetTick(void)
{
	return Asys.g_tick_count;
}

ITCM_AREA_CODE uint32_t HAL_GetTick(void)
{
uint32_t	ret_val;
	if ( Asys.g_os_started )
		ret_val =  Asys.g_tick_count;
	else
		ret_val =  uwTick;
	return ret_val;
}

#define	NEW_TASK_DELAY	1
ITCM_AREA_CODE void task_delay(uint32_t tick_count)
{
	__disable_irq();
	if(Asys.current_process)
	{
		process[Asys.current_process].delay_value = Asys.g_tick_count + tick_count;
		process[Asys.current_process].current_state &= ~PROCESS_READY_STATE;
#ifdef NEW_TASK_DELAY
		process[Asys.current_process].wait_event |= SUSPEND_ON_DELAY;
		process[Asys.current_process].wakeup_rsn &= ~SUSPEND_ON_DELAY;
		while((process[Asys.current_process].wakeup_rsn & WAKEUP_FROM_DELAY) != WAKEUP_FROM_DELAY )
			schedule();
		process[Asys.current_process].wakeup_rsn &= ~SUSPEND_ON_DELAY;
		process[Asys.current_process].wakeup_flags &= ~SUSPEND_ON_DELAY;
#else
		process[Asys.current_process].wait_event = SUSPEND_ON_DELAY;
		schedule();
#endif
	}
	__enable_irq();
}

ITCM_AREA_CODE void check_timers(void)
{
register uint8_t	i,j;

	for( i = 1 ; i < MAX_PROCESS ; i++)
	{
		if((process[i].wait_event & SUSPEND_ON_DELAY) == SUSPEND_ON_DELAY)
		{
			if(Asys.g_tick_count >= process[i].delay_value)
			{
#ifdef NEW_TASK_DELAY
				activate_process(i,WAKEUP_FROM_DELAY,WAKEUP_FROM_DELAY);
#else
				process[i].current_state |= PROCESS_READY_STATE;
#endif
			}
		}

		if((process[i].wait_event & SUSPEND_ON_TIMER) == SUSPEND_ON_TIMER)
		{
			for( j = 0 ; j < MAX_TIMERS ; j++)
			{
				if((process[i].timer_flags[j] & TIMERFLAGS_IN_USE ) == TIMERFLAGS_IN_USE)
				{
					if((process[i].timer_flags[j] & TIMERFLAGS_ENABLED ) == TIMERFLAGS_ENABLED)
					{
						if(Asys.g_tick_count >= process[i].current_timer[j] )
						{
							process[i].current_state |= PROCESS_READY_STATE;
							if ((process[i].timer_flags[j] & TIMERFLAGS_FOREVER ) == TIMERFLAGS_FOREVER)
								process[i].current_timer[j] = Asys.g_tick_count + process[i].timer_value[j];
							else
								process[i].timer_flags[j] &= ~TIMERFLAGS_ENABLED;
							process[i].timer_expired |= (1<<j);
							activate_process(i,WAKEUP_FROM_TIMER,1<<j);
						}
					}
					else
						process[i].current_timer[j]++;
				}
			}
		}
	}
}

ITCM_AREA_CODE void  SysTick_Handler(void)
{
	__disable_irq();
	if ( Asys.g_os_started )
	{
		update_global_tick_count();
		if ( before_check_timers_callback != NULL)
			before_check_timers_callback();

		check_timers();

		if ( after_check_timers_callback != NULL)
			after_check_timers_callback();

		//pend the pendsv exception
		schedule();
	}
	else
	{
		HAL_IncTick();
		HAL_SYSTICK_IRQHandler();
	}
	__enable_irq();
}

ITCM_AREA_CODE uint32_t create_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags)
{
uint8_t timer_index = 0;
	if (( process[Asys.current_process].timer_flags[timer_id] & TIMERFLAGS_IN_USE ) == TIMERFLAGS_IN_USE)
		return 1;
	switch(timer_id)
	{
	case	TIMER_ID_0:	timer_index = 0; break;
	case	TIMER_ID_1:	timer_index = 1; break;
	case	TIMER_ID_2:	timer_index = 2; break;
	case	TIMER_ID_3:	timer_index = 3; break;
	case	TIMER_ID_4:	timer_index = 4; break;
	case	TIMER_ID_5:	timer_index = 5; break;
	case	TIMER_ID_6:	timer_index = 6; break;
	case	TIMER_ID_7:	timer_index = 7; break;
	default : return 1;
	}
	__disable_irq();
	process[Asys.current_process].timer_flags[timer_index] = (flags & TIMERFLAGS_USERMASK) | TIMERFLAGS_IN_USE;
	process[Asys.current_process].timer_value[timer_index] = tick_count;
	process[Asys.current_process].current_timer[timer_index] = Asys.g_tick_count + tick_count;
	__enable_irq();
	return 0;
}

ITCM_AREA_CODE uint32_t start_timer(uint8_t timer_id)
{
uint8_t timer_index = 0;
	if (( process[Asys.current_process].timer_flags[timer_id] & TIMERFLAGS_IN_USE ) == TIMERFLAGS_IN_USE)
		return 1;
	switch(timer_id)
	{
	case	TIMER_ID_0:	timer_index = 0; break;
	case	TIMER_ID_1:	timer_index = 1; break;
	case	TIMER_ID_2:	timer_index = 2; break;
	case	TIMER_ID_3:	timer_index = 3; break;
	case	TIMER_ID_4:	timer_index = 4; break;
	case	TIMER_ID_5:	timer_index = 5; break;
	case	TIMER_ID_6:	timer_index = 6; break;
	case	TIMER_ID_7:	timer_index = 7; break;
	default : return 1;
	}
	process[Asys.current_process].timer_flags[timer_index] |= TIMERFLAGS_ENABLED;
	return 0;
}

ITCM_AREA_CODE uint32_t restart_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags)
{
	if (( process[Asys.current_process].timer_flags[timer_id] & TIMERFLAGS_IN_USE ) != TIMERFLAGS_IN_USE)
		return 1;
	process[Asys.current_process].timer_flags[timer_id] &= ~TIMERFLAGS_IN_USE;
	create_timer(timer_id,tick_count,flags);
	return 0;
}

ITCM_AREA_CODE uint32_t stop_timer(uint8_t timer_id)
{
	process[Asys.current_process].timer_flags[timer_id] &= ~TIMERFLAGS_ENABLED;
	return 0;
}

ITCM_AREA_CODE uint32_t destroy_timer(uint8_t timer_id)
{
	__disable_irq();
	process[Asys.current_process].timer_flags[timer_id] = 0;
	__enable_irq();
	return 0;
}

ITCM_AREA_CODE uint8_t get_timer_expired(void)
{
uint8_t tim_exp = process[Asys.current_process].timer_expired;
	process[Asys.current_process].timer_expired &= ~tim_exp;
	return tim_exp;
}

