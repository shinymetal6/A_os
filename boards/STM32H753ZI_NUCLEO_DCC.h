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
 * STM32H753ZI_NUCLEO_DCC.h
 *
 *  Created on: Oct 16, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32H753ZI_NUCLEO_DCC_H_
#define BOARDS_STM32H753ZI_NUCLEO_DCC_H_

#include	"boards_common_mem.h"

/* I/O */
#define	LED_1_GPIOPORT			LD1_GPIO_Port
#define	LED_1_GPIOBIT			LD1_Pin
#define	LED_2_GPIOPORT			LD2_GPIO_Port
#define	LED_2_GPIOBIT			LD2_Pin
#define	LED_3_GPIOPORT			LD3_GPIO_Port
#define	LED_3_GPIOBIT			LD3_Pin
#define	BUTTON_GPIOPORT			B1_GPIO_Port
#define	BUTTON_GPIOBIT			B1_Pin

#define	NETWORKING_ENABLED		1
#define	USB_DEVICE_ENABLED		1
//#define	XMODEM_ENABLE			1
//#define	MODBUS_ENABLE			1
#define MQTT_ENABLE				1

//#define	INTERNAL_ADC_DRIVER			1
//#define	INTERNAL_DAC_DRIVER			1
//#define	DHTXX_AM230X_ENABLE		1
//#define	MOTOR_CNTRL_A			1
//#define	MOTOR_CNTRL_B			1
#define	DCC_SYSTEM_ENABLE		1

#ifdef DHTXX_AM230X_ENABLE
	extern	TIM_HandleTypeDef 				htim2;
	#define DHTXX_AM230X_TIMER				htim2
	#define DHTXX_AM230X_TIM_CHANNEL		TIM_CHANNEL_4
	#define	GPIOPORT_DHTXX_AM230X			GPIOA
	#define	GPIOBIT_DHTXX_AM230X			3
#endif // #ifdef DHTXX_AM230X_ENABLE

#ifdef MOTOR_CNTRL_A
	extern	TIM_HandleTypeDef 				htim1;
	#define MOTOR_CNTRL_PWM_A				htim1
	#define MOTOR_CNTRL_DIRECT_PWM_A		TIM1
	#define MOTOR_CNTRL_CHANNEL_A			TIM_CHANNEL_3
	#define MOTOR_CNTRL_PWM_A_PERIOD		10000
	#define MOTOR_CNTRL_DIR_A_GPIOPORT		DIR_A_GPIO_Port
	#define MOTOR_CNTRL_DIR_A_GPIOBIT		DIR_A_Pin
	#define MOTOR_CNTRL_BRAKE_A_GPIOPORT	BRAKE_A_GPIO_Port
	#define MOTOR_CNTRL_BRAKE_A_GPIOBIT		BRAKE_A_Pin
	#define	A_HAS_TIMER2					1
#endif

#ifdef MOTOR_CNTRL_B
	extern	TIM_HandleTypeDef 				htim3;
	#define MOTOR_CNTRL_PWM_B				htim3
	#define MOTOR_CNTRL_CHANNEL_B			TIM_CHANNEL_2
	#define MOTOR_CNTRL_PWM_B_PERIOD		10000
	#define MOTOR_CNTRL_DIR_B_GPIOPORT		DIR_B_GPIO_Port
	#define MOTOR_CNTRL_DIR_B_GPIOBIT		DIR_B_Pin
	#define MOTOR_CNTRL_BRAKE_B_GPIOPORT	BRAKE_B_GPIO_Port
	#define MOTOR_CNTRL_BRAKE_B_GPIOBIT		BRAKE_B_Pin
	#define	A_HAS_TIMER3					1
#endif

#if defined(MOTOR_CNTRL_A) || defined(MOTOR_CNTRL_B)
	#define	A_HAS_MOTOR_CNTRL				1
	#if defined(MOTOR_CNTRL_A) && defined(MOTOR_CNTRL_B)
		#define	A_HAS_MOTOR_CNTRL_NUMBER				2
	#else
		#define	A_HAS_MOTOR_CNTRL_NUMBER				1
	#endif
#endif


#ifdef DCC_SYSTEM_ENABLE
	#define	A_HAS_TIMER1			1
	extern	TIM_HandleTypeDef 		htim1;
	#define DCC_TIMER				htim1
	#define DCC_CHANNEL_OUT			TIM_CHANNEL_3
	#define DCC_CHANNEL_CUTOUT		TIM_CHANNEL_4
	#define DCC_ENABLE_PORT			DCC_ENABLE_GPIO_Port
	#define DCC_ENABLE_GPIOBIT		DCC_ENABLE_Pin
	#define	DCC_TIMER_DUAL_PHASE	1
#endif // #ifdef DCC_SYSTEM_ENABLE

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

#define	BOARD_NAME			"STM32H753ZI_NUCLEO_DCC"
#define	MACHINE_NAME		"STM32H753ZI_NUCLEO_DCC"
#define	MACHINE_VERSION		"A"


#endif /* BOARDS_STM32H753ZI_NUCLEO_DCC_H_ */
