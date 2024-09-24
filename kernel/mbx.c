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
 * mbx.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */
#include "main.h"
#include "A.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"
#include <string.h>

extern	PCB_t process[MAX_PROCESS];

ITCM_AREA_CODE void mbx_send(uint8_t process_number,uint8_t mailbox_number,uint8_t *mbx_ptr,uint32_t mbx_size)
{
	process[process_number].mbx[mailbox_number] = mbx_ptr;
	process[process_number].mbx_size[mailbox_number] = mbx_size;
	process[process_number].mbx_bits |= (1 << mailbox_number);
	activate_process(process_number,WAKEUP_FROM_MBX,process[process_number].mbx_bits);
}

ITCM_AREA_CODE uint32_t mbx_receive(uint8_t mailbox_number,uint8_t *buf_ptr)
{
	if (process[Asys.current_process].mbx_bits & (1 << mailbox_number) )
	{
		memcpy(buf_ptr,process[Asys.current_process].mbx[mailbox_number] ,process[Asys.current_process].mbx_size[mailbox_number]);
		process[Asys.current_process].wakeup_rsn &= ~WAKEUP_FROM_MBX;
		process[Asys.current_process].mbx_bits &= ~(1 << mailbox_number);
		return process[Asys.current_process].mbx_size[mailbox_number];
	}
	return 0;
}


