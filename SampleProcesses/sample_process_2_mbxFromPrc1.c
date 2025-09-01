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
 * sample_process_2_mbxFromPrc1.c
 *
 *  Created on: Jul 25, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_2_MBX_FROM_PRC1

#define	PRC1_MAILBOX	0

uint8_t		mbx_from_prc1[sizeof(uint32_t)];

void sample_process_2_mbxFromPrc1(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	mbx_size;

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event( EVENT_TIMER | EVENT_MBX );
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
		}
		if (( wakeup & WAKEUP_FROM_MBX) == WAKEUP_FROM_MBX)
		{
			mbx_size = mbx_receive(PRC1_MAILBOX,mbx_from_prc1);
			if ( mbx_size )
			{
				if ( (mbx_from_prc1[0] == 0x55) && (mbx_from_prc1[1] == 0xaa) && (mbx_from_prc1[2] == 0x7e) && (mbx_from_prc1[3] == 0xe7) )
					process_led();
			}
		}
	}
}
#endif // #ifdef 	SAMPLEPROCESS_2_MBX_FROM_PRC1

#endif // #ifdef SAMPLE_PROCESSES_ENABLED
