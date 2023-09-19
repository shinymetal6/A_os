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
 * Project : A_os_U575_devel 
*/
/*
 * semaphores.c
 *
 *  Created on: Sep 19, 2023
 *      Author: fil
 */
#include "main.h"
#include "A.h"
#include "scheduler.h"
#include "A_exported_functions.h"

extern	Semaphores_t	Semaphores;

uint32_t destroy_semaphore(uint8_t semaphore_id)
{
	Semaphores.semaphore[semaphore_id] = 0;
	if ( Semaphores.semaphore_waiting_process[semaphore_id] )
		activate_process(Semaphores.semaphore_waiting_process[semaphore_id],WAKEUP_FROM_SEMAPHORE,0);

	return 0;
}

uint32_t get_semaphore(uint8_t semaphore_id,uint8_t semaphore_flag,uint32_t semaphore_timeout)
{
uint32_t	ret_val = SEMAPHORE_RED;
	if ( (semaphore_flag & SEMAPHORE_SUSPEND_IF_RED) != SEMAPHORE_SUSPEND_IF_RED)
	{
		if ((Semaphores.semaphore[semaphore_id] ==  SEMAPHORE_GREEN ) || (Semaphores.semaphore[semaphore_id] ==  Asys.current_process ))
		{
			Semaphores.semaphore[semaphore_id] = Asys.current_process;
			Semaphores.semaphore_timeout[semaphore_id] = Asys.g_tick_count + semaphore_timeout;
			ret_val = SEMAPHORE_GREEN;
		}
	}
	else
	{
		if ((Semaphores.semaphore[semaphore_id] ==  SEMAPHORE_GREEN ) || (Semaphores.semaphore[semaphore_id] ==  Asys.current_process ))
		{
			Semaphores.semaphore[semaphore_id] = Asys.current_process;
			Semaphores.semaphore_timeout[semaphore_id] = Asys.g_tick_count + semaphore_timeout;
			ret_val = SEMAPHORE_GREEN;
		}
		else
		{
			if ( Semaphores.semaphore_waiting_process[semaphore_id] == 0 )
			{
				Semaphores.semaphore_waiting_process[semaphore_id] = Asys.current_process;
				if ( semaphore_timeout )
				{
					Semaphores.semaphore_timeout[semaphore_id] = Asys.g_tick_count + semaphore_timeout;
					wait_event( EVENT_SEMAPHORE_TIMEOUT );
				}
				else
					wait_event(EVENT_SEMAPHORE );
				Semaphores.semaphore[semaphore_id] = Asys.current_process;
				ret_val = SEMAPHORE_GREEN;
			}
			else
				ret_val = SEMAPHORE_UNAVAILABLE;
		}
	}
	return ret_val;
}

