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
 * AX_P2416176_01_NEVOL.h
 *
 *  Created on: May 23, 2024
 *      Author: fil
 */

#ifndef BOARDS_AX_P2416176_01_NEVOL_H_
#define BOARDS_AX_P2416176_01_NEVOL_H_

#include "memdef_common.h"

#define USB_DEVICE_ENABLED	1
#ifdef USB_DEVICE_ENABLED
	#define	USB_CDC				1
	//#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED

//#define	 QSPI_ENABLED	1

#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))
#define DWNLD_DATA_AREA		__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))
#define ETH_DATA_AREA		__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))

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

#define	BOARD_NAME			"NEVOL-2416176-01"
#define	MACHINE_NAME		"Nevol"
#define	MACHINE_VERSION		"A"


#endif /* BOARDS_AX_P2416176_01_NEVOL_H_ */
