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

#include "sample_processes_includes.h"

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

#ifdef	SAMPLEPROCESS_1_PING_NRF24L01
	#define	WIRELESS_NRF24L01		1
#endif
#ifdef SAMPLEPROCESS_1_MIDI
	#define	MIDI_ENABLED	1
#endif
#ifdef SAMPLEPROCESS_1_AUDIO_DAC_MIDI
	#define	SOUND_ENGINE_ENABLED	1
	#define	MIDI_ENABLED	1
#endif
#ifdef SAMPLEPROCESS_1_AUDIO_DAC_OSCILLATOR
	#define	SOUND_ENGINE_ENABLED	1
#endif

#ifdef AU100825
	#ifdef SAMPLEPROCESS_1_AUDIO_I2S_DUAL_OSCILLATOR
		#define	SOUND_ENGINE_ENABLED	1
		#define	SOUND_ENGINE_I2S_ENABLED	1
	#endif
	#ifdef SAMPLEPROCESS_1_AUDIO_I2S_OSCILLATOR
		#define	SOUND_ENGINE_ENABLED	1
		#define	SOUND_ENGINE_I2S_ENABLED	1
	#endif
	#ifdef SAMPLEPROCESS_1_AUDIO_I2S_MIDI
		#define	SOUND_ENGINE_ENABLED	1
		#define	SOUND_ENGINE_I2S_ENABLED	1
		#define	MIDI_ENABLED	1
	#endif
	#ifdef SAMPLEPROCESS_1_AUDIO_I2S_DRUM
		#define	SOUND_ENGINE_ENABLED	1
		#define	SOUND_ENGINE_I2S_ENABLED	1
		#define	MIDI_ENABLED	1
	#endif
	#ifdef SAMPLEPROCESS_1_AUDIO_I2S_IN_EFFECT_OUT
		#define	SOUND_ENGINE_ENABLED	1
		#define	SOUND_ENGINE_I2S_ENABLED	1
		#define	MIDI_ENABLED	1
	#endif
	#ifdef SAMPLEPROCESS_1_SDCARD
		#define	SOUND_ENGINE_ENABLED	1
		#define	SOUND_ENGINE_I2S_ENABLED	1
		#define	MIDI_ENABLED	1
	#endif
	#ifdef SAMPLEPROCESS_1_LCD7735
		#define	LCD_ENABLED	1
	#endif
#endif // #ifdef AU100825

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

//#define	LCD_ENABLED		1

#ifdef SAMPLEPROCESS_1_MIDI
#define USB_DEVICE_ENABLED		1
#endif
#ifdef SAMPLEPROCESS_1_XMODEM_RX_USB
#define USB_DEVICE_ENABLED		1
#endif
#ifdef SAMPLEPROCESS_1_SWITCH_MIDI_CDC
#define USB_DEVICE_ENABLED		1
#endif
#ifdef SAMPLEPROCESS_1_DCCPWM
#define USB_DEVICE_ENABLED		1
#endif
#if defined (USB_OTG_FS) || defined (USB_OTG_HS)
	#define USB_DEVICE_ENABLED		1
#endif
#if defined (HAL_PCD_MODULE_ENABLED) || defined (HAL_HCD_MODULE_ENABLED)
#if defined (USB)
#define USB_DEVICE_ENABLED		1
#endif
#endif

#ifdef USB_DEVICE_ENABLED
	#ifdef MIDI_ENABLED
		#define USBD_MIDI_MANUFACTURER_STRING		"Nucleo743"
		#define USBD_MIDI_PRODUCT_STRING_FS			"NucleoH743_Midi"
		#define	USB_MIDI			1
	#else
		#define	USB_CDC				1
	#endif
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
