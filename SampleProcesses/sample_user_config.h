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
 * sample_user_config.h
 *
 *  Created on: Sep 1, 2025
 *      Author: fil
 */

#ifndef PROJECT_LIB_MODULES_H_
#define PROJECT_LIB_MODULES_H_

/* OS settings */
/* SCHED_STD set means the activate_process function checks if the process is waiting on a specific flag
 * If SCHED_STD is not set then all the wakeup flags will wake up the process
 */
#define	SCHED_STD		1
#ifndef SCHED_STD
	#define	SCHED_NO_MASK		1
#endif // #ifdef SCHED_STD

/* Libraries */
//#define	HELIX_ENABLE	1
//#define	LORAWAN_ENABLE	1
//#define	LWIP_ENABLE		1
//#define	WIFI_ENABLE		1
#define	WIRELESS_NRF24L01		1

#define	POWERSAVING_ENABLED	1

#ifdef LWIP_ENABLE
	#define	NETWORKING_ENABLED		1
	//#define	MQTT_ENABLE	1
#endif // #ifdef LWIP_ENABLE
#ifdef WIFI_ENABLE
	#define	WIFI_ESP01S		1
#endif // #ifdef WIFI_ENABLE

/* Modules */
//#define	HEX_DEC_ENABLE	1
#define	XMODEM_ENABLE	1
//#define	MODBUS_ENABLE	1
//#define	SOUND_ENABLED	1

/* drivers */

#define USB_DEVICE_ENABLED		1
#ifdef USB_DEVICE_ENABLED
	#define	USB_CDC				1
	//#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED

#ifndef LED_GPIO_Port
	#ifdef LD1_GPIO_Port
		#define LED_Pin			LD1_Pin
		#define LED_GPIO_Port	LD1_GPIO_Port
	#else //#ifdef LD1_GPIO_Port
	#ifdef LD2_GPIO_Port
		#define LED_Pin			LD2_Pin
		#define LED_GPIO_Port	LD2_GPIO_Port
	#endif //#ifdef LD1_GPIO_Port
	#endif //#ifdef LD1_GPIO_Port
#endif

#define	BOARD_NAME			"Board"
#define	MACHINE_NAME		"Board"
#define	MACHINE_VERSION		"Sample Processes"

#endif /* PROJECT_LIB_MODULES_H_ */
