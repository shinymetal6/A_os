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
 * svc.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */


#include "main.h"
#include "A.h"
#include "system_default.h"
#include "scheduler.h"
#include "A_exported_functions.h"

SYSTEM_RAM	uint8_t	opcode;
SYSTEM_RAM	void 	(*svc_call_array[MAX_SVC])(void);

__attribute__((naked)) void svc_call(uint8_t svc_num)
{
	if ( svc_call_array[svc_num] == NULL )
		__asm volatile("bx lr");
	if ( svc_num >= MAX_SVC )
		__asm volatile("bx lr");
	switch(svc_num)
	{
	case 0 :	__asm volatile("SVC #0"); __asm volatile("bx lr");
	case 1 :	__asm volatile("SVC #1"); __asm volatile("bx lr");
	case 2 :	__asm volatile("SVC #2"); __asm volatile("bx lr");
	case 3 :	__asm volatile("SVC #3"); __asm volatile("bx lr");
	case 4 :	__asm volatile("SVC #4"); __asm volatile("bx lr");
	case 5 :	__asm volatile("SVC #5"); __asm volatile("bx lr");
	case 6 :	__asm volatile("SVC #6"); __asm volatile("bx lr");
	case 7 :	__asm volatile("SVC #7"); __asm volatile("bx lr");
	case 8 :	__asm volatile("SVC #8"); __asm volatile("bx lr");
	case 9 :	__asm volatile("SVC #9"); __asm volatile("bx lr");
	case 10 :	__asm volatile("SVC #10"); __asm volatile("bx lr");
	case 11 :	__asm volatile("SVC #11"); __asm volatile("bx lr");
	case 12 :	__asm volatile("SVC #12"); __asm volatile("bx lr");
	case 13 :	__asm volatile("SVC #13"); __asm volatile("bx lr");
	case 14 :	__asm volatile("SVC #14"); __asm volatile("bx lr");
	case 15 :	__asm volatile("SVC #15"); __asm volatile("bx lr");
	}
}

void SVC_Handler_init(void (*function),uint8_t idx)
{
	if ( idx < MAX_SVC )
		svc_call_array[idx] = (void *)function;
}

void SVC_Handler_c(uint32_t *pEStack)
{
	opcode = ( ( char * )pEStack[ 6 ] )[ -2 ] ;
	if ( opcode < MAX_SVC )
		svc_call_array[opcode]();
}

__attribute__ ((naked)) void SVC_Handler(void)
{
//by convention func is in r0 and args is in r1
	__asm volatile("TST lr, #4");
	__asm volatile("ITE EQ");
	__asm volatile("MRSEQ r0, MSP");
	__asm volatile("MRSNE r0, PSP");
	__asm volatile("B SVC_Handler_c");
}
