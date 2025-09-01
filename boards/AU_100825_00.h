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
 * AU_100825_00.h
 *
 *  Created on: Aug 26, 2025
 *      Author: fil
 */

#ifndef BOARDS_AU_100825_00_H_
#define BOARDS_AU_100825_00_H_

#include "iodef_common.h"
#include "memdef_common.h"

#define USB_DEVICE_ENABLED	1
#ifdef USB_DEVICE_ENABLED
	//#define	USB_CDC				1
	#define	USB_MIDI
	#ifdef USB_MIDI
		#define USBD_MANUFACTURER_STRING		"BB"
		#define USBD_PRODUCT_STRING_FS			"BB AU100825"
	#endif
#endif // #ifdef USB_ENABLED

#define SOUND_ENABLED	1
//#define LORA_ENABLED	1

#define HAS_LCD			1
#ifdef HAS_LCD
	#define	BOARD_LCD	LCD_IS_7735
	//#define	BOARD_LCD	LCD_IS_9341
#endif //#ifdef HAS_LCD

#define LD1_Pin			LED_Pin
#define LD1_GPIO_Port	LED_GPIO_Port

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

#define	BOARD_NAME			"AU_100825_00"
#define	MACHINE_NAME		"BB"
#define	MACHINE_VERSION		"A"

#endif /* BOARDS_AU_100825_00_H_ */
