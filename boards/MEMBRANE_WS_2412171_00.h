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
 * MEMBRANE_WS_2412171_00.h
 *
 *  Created on: Feb 15, 2025
 *      Author: fil
 */

#ifndef BOARDS_MEMBRANE_WS_2412171_00_H_
#define BOARDS_MEMBRANE_WS_2412171_00_H_

#include "iodef_common.h"
#include "memdef_common.h"

#define DTCM_VECTORS_DATA
#define DWNLD_DATA_AREA
#define ETH_DATA_AREA
#define FRAME_BUFFER
#define BOARDPARAMETERS_AREA		__attribute__((section(".board_paramStart"))) __attribute__ ((aligned (32)))
#define BOARDINFO_DATA_AREA			__attribute__((section(".board_infoStart"))) __attribute__ ((aligned (32)))
#define FLASHDATARAM_AREA			__attribute__((section(".FlashDataRamStart"))) __attribute__ ((aligned (32)))

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

#define	BOARD_NAME			"MEMBRANE_2412171_00"
#define	MACHINE_NAME		"III"
#define	MACHINE_VERSION		"A"


#endif /* BOARDS_MEMBRANE_WS_2412171_00_H_ */
