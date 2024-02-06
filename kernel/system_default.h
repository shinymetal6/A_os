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
 * system_default.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_SYSTEM_DEFAULT_H_
#define KERNEL_SYSTEM_DEFAULT_H_

#ifdef	STM32H743xx
	#ifdef STM32H743ZI2_NUCLEO
		#include "../boards/STM32H743ZI2_NUCLEO.h"
	#endif
	#ifdef	FY_201023_00
		#include "../boards/FY_201023_00.h"
	#endif
	#ifdef	BB2xx_743
		#include "../boards/BB2xx_743.h"
	#endif
#endif	//#ifdef	STM32H743xx

#ifdef	STM32H563xx
	#ifdef STM32H563ZI2_NUCLEO
		#include "../boards/STM32H563ZI_NUCLEO.h"
	#endif
#endif

#ifndef ASSIGNED
#error "Processor not implemented"
#endif

/* common for all processors */
#define	POOL_CHUNK_SIZE		    256
#define	POOL_NUM			    32
#define	POOL_SIZE			    (POOL_CHUNK_SIZE*POOL_NUM)
#define	POOL_END			    (POOL_START+POOL_SIZE)
#define SRAM_END                 ((SRAM_START) + (SRAM_SIZE) )
#define SIZE_SCHED_STACK         4096U
#define SCHED_STACK_START        SRAM_END
#define IDLE_STACK_START         (SCHED_STACK_START - SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          4096U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)
#define SYSTEM_RAM				__attribute__((section(".osSysRam"))) __attribute__ ((aligned (32)))
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	Exti0_PRIORITY			13
#define	Exti1_PRIORITY			13
#define	Exti2_PRIORITY			13


#endif /* KERNEL_SYSTEM_DEFAULT_H_ */
