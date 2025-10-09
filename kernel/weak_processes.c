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
 * weak_processes.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */
#include "main.h"
#include "A.h"
#include "A_exported_functions.h"

/* These processes waste only time, they are defined to have something of useless to schedule */
__weak void weak_process1_init(uint32_t process_id)
{
	while(1);
}
__weak void weak_process2_init(uint32_t process_id)
{
	while(1);
}
__weak void weak_process3_init(uint32_t process_id)
{
	while(1);
}
__weak void weak_process4_init(uint32_t process_id)
{
	while(1);
}

__weak void weak_process1(uint32_t process_id)
{
	while(1);
}
__weak void weak_process2(uint32_t process_id)
{
	while(1);
}
__weak void weak_process3(uint32_t process_id)
{
	while(1);
}
__weak void weak_process4(uint32_t process_id)
{
	while(1);
}

/* This processes table is define as weak so can be overridden by a user defined processes table */
__weak USRprcs_t	UserProcesses[USR_PROCESS_NUMBER] =
{
		{
				.user_process = weak_process1,
				.user_init = weak_process1_init,
				.stack_size = 256,
		},
		{
				.user_process = weak_process2,
				.user_init = weak_process2_init,
				.stack_size = 256,
		},
		{
				.user_process = weak_process3,
				.user_init = weak_process3_init,
				.stack_size = 256,
		},
		{
				.user_process = weak_process4,
				.user_init = weak_process4_init,
				.stack_size = 256,
		}
};



