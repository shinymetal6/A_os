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
 * PRESSO_2416176_01.h
 *
 *  Created on: Oct 31, 2024
 *      Author: fil
 */
#ifndef BOARDS_PRESSO_2416176_01_H_
#define BOARDS_PRESSO_2416176_01_H_

#include	"boards_common_mem.h"

//#define	NETWORKING_ENABLED		1
#define	USB_DEVICE_ENABLED		1
//#define	MODBUS_ENABLE			1
#define MQTT_ENABLE				1

#ifdef NETWORKING_ENABLED
// 1 starts dhcp, 0 means fixed IP defined in A.c
	//#define	NETWORKING_DHCP				1
#endif // #ifdef NETWORKING_ENABLED

#ifdef USB_DEVICE_ENABLED
	#define	USB_CDC				1
	//#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED

#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))
#define XMODEM_DATA_AREA	__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))
//#define ETH_DATA_AREA		__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))
#define SAMPLES_AREA		__attribute__((section(".aos_flash2")))  __attribute__ ((aligned (32)))

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

#define	BOARD_NAME			"PRESSO_2416176_01"
#define	MACHINE_NAME		"PRESSO_2416176_01"
#define	MACHINE_VERSION		"A"



#endif /* BOARDS_PRESSO_2416176_01_H_ */
