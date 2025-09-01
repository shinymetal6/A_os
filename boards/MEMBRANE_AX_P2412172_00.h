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
 * MEMBRANE_AX_P2412172_00.h
 *
 *  Created on: Sep 1, 2025
 *      Author: fil
 */

#ifndef BOARDS_MEMBRANE_AX_P2412172_00_H_
#define BOARDS_MEMBRANE_AX_P2412172_00_H_

/*
 * for this board define
 * MEMBRANE_2412172_00
 * in preprocessor defines
 */

#include "iodef_common.h"
#include "memdef_common.h"

#if __has_include("project_lib_modules.h")
	#include "project_lib_modules.h"
#else
	#error "No user project_lib_modules.h found in user space"
#endif



#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))
#define DWNLD_DATA_AREA		__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))
#define ETH_DATA_AREA		__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))
#define FRAME_BUFFER		__attribute__((section(".framebuffer"))) 	__attribute__ ((aligned (32)))

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		480000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1


#define	DFU_BOOT_ENTRY			0x1FF09800
#define	DFU_BOOT_VERSION_PTR	0x1FF1E7FE
#define	DFU_BOOT_VERSION		0x91

#define	BOARD_NAME			"Concentrator-2412172_00"
#define	MACHINE_NAME		"Membrane"
#define	MACHINE_VERSION		"A"


#endif /* BOARDS_MEMBRANE_AX_P2412172_00_H_ */
