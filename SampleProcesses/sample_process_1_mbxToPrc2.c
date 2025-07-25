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
 * sample_process_1_mbxToPrc2.c
 *
 *  Created on: Jul 25, 2025
 *      Author: fil
 */
#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_MBX_TO_PRC2

uint8_t		mbx_to_prc2[sizeof(uint32_t)];

#define	PRC2_ID			2
#define	PRC2_MAILBOX	0

void sample_process_1_mbxToPrc2(uint32_t process_id)
{
uint32_t	wakeup,flags;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event( EVENT_TIMER | EVENT_MBX );
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			mbx_to_prc2[0] = 0x55;
			mbx_to_prc2[1] = 0xaa;
			mbx_to_prc2[2] = 0x7e;
			mbx_to_prc2[3] = 0xe7;
			mbx_send(PRC2_ID,PRC2_MAILBOX,mbx_to_prc2,sizeof(uint32_t));
		}
	}
}
#endif // #ifdef 	SAMPLEPROCESS_1_MBX_TO_PRC2

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

