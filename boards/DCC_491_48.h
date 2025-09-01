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
 * DCC_491_48.h
 *
 *  Created on: Jan 8, 2025
 *      Author: fil
 */
#ifndef BOARDS_DCC_491_48_H_
#define BOARDS_DCC_491_48_H_

/*
 * for this board define
 * DCC_491_48
 * in preprocessor defines
 */

#include "iodef_common.h"
#include "memdef_common.h"

#define STM32G4xx_USB	1
#if __has_include("project_lib_modules.h")
	#include "project_lib_modules.h"
#else
	#error "No user project_lib_modules.h found in user space"
#endif
#define USB_DEVICE_ENABLED	1
#ifdef USB_DEVICE_ENABLED
	#define	USB_CDC				1
	//#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED

#define DTCM_VECTORS_DATA
#define DWNLD_DATA_AREA
#define ETH_DATA_AREA
#define FRAME_BUFFER

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		170000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1


#define	DFU_BOOT_ENTRY			0x1FF09800
#define	DFU_BOOT_VERSION_PTR	0x1FF1E7FE
#define	DFU_BOOT_VERSION		0x91

#define	BOARD_NAME			"DCC_491_48_00"
#define	MACHINE_NAME		"III"
#define	MACHINE_VERSION		"A"

#endif /* BOARDS_DCC_491_48_H_ */
