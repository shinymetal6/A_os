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
 * sample_process_1_g431rb_pn5180.c
 *
 *  Created on: Jan 12, 2026
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef 	SAMPLEPROCESS_1_G431RB_PN5180
void sample_process_1_init(uint32_t process_id)
{

}
void sample_process_1_g431rb_pn5180(uint32_t process_id)
{
uint32_t	wakeup,flags;

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
		}
	}
}
#endif // #ifdef 	SAMPLEPROCESS_1_PN5180

#endif // #ifdef SAMPLE_PROCESSES_ENABLED
