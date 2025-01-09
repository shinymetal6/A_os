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
 * STM32H753ZI_NUCLEO.h
 *
 *  Created on: Apr 19, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32H753ZI_NUCLEO_H_
#define BOARDS_STM32H753ZI_NUCLEO_H_

/*
 * for this board define
 * STM32H753ZI_NUCLEO
 * in preprocessor defines
 */

#include "iodef_common.h"
#include "memdef_common.h"

//#define	NETWORKING_ENABLED		1
//#define	USB_DEVICE_ENABLED		1

#ifdef NETWORKING_ENABLED
	#define MQTT_ENABLE				1
	//#define	NETWORKING_DHCP				1	/* 1 starts dhcp, 0 means fixed IP defined in A.c */
#endif // #ifdef NETWORKING_ENABLED

#ifdef USB_DEVICE_ENABLED
	#define	USB_CDC				1
	//#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED


#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))
#define XMODEM_DATA_AREA	__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))
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

#define	BOARD_NAME			"STM32H753ZI_NUCLEO"
#define	MACHINE_NAME		"STM32H753ZI_NUCLEO"
#define	MACHINE_VERSION		"A"


#endif /* BOARDS_STM32H753ZI_NUCLEO_H_ */
