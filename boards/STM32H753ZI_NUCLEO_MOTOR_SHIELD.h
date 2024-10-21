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
 * STM32H753ZI_NUCLEO_MOTOR_SHIELD.h
 *
 *  Created on: Oct 14, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32H753ZI_NUCLEO_MOTOR_SHIELD_H_
#define BOARDS_STM32H753ZI_NUCLEO_MOTOR_SHIELD_H_

#include	"boards_common_mem.h"
/* Memories */
/*
// memory pool defines
extern	uint8_t					*_mempool_start,*_mempool_end;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    256
#define	POOL_SIZE			    (POOL_END - POOL_START)
// POOL_NUM must be a constant value to compile
#define	POOL_NUM			    32
#define POOL_ENABLE				1

// system defines
extern	uint8_t					*_osSysRam_start,*_osSysRam_end;
#define SRAM_START				(uint32_t )(&_osSysRam_start)
#define SRAM_END				(uint32_t )(&_osSysRam_end)
#define SRAM_SIZE				(&_osSysRam_end - &_osSysRam_start)
#define SIZE_SCHED_STACK		4096U
#define SCHED_STACK_START		SRAM_END

#define IDLE_STACK_START		(uint32_t )((uint32_t )&_osSysRam_end - (uint32_t )SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          4096U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)
*/
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
//#define	DDC_SYSTEM_ENABLE		1

//#define	DHTXX_AM230X_ENABLE			1
#ifdef DHTXX_AM230X_ENABLE
	extern	TIM_HandleTypeDef 				htim2;
	#define DHTXX_AM230X_TIMER				htim2
	#define DHTXX_AM230X_TIM_CHANNEL		TIM_CHANNEL_4
	#define	GPIOPORT_DHTXX_AM230X			GPIOA
	#define	GPIOBIT_DHTXX_AM230X			3
#endif // #ifdef DHTXX_AM230X_ENABLE

#define	MOTOR_CNTRL_A			1
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

#define	MOTOR_CNTRL_B			1
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

#ifdef NETWORKING_ENABLED
// 1 starts dhcp, 0 means fixed IP defined in A.c
	//#define	NETWORKING_DHCP				1
#endif // #ifdef NETWORKING_ENABLED

#ifdef USB_DEVICE_ENABLED
	#define	USB_CDC				1
	//#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED

#ifdef DDC_SYSTEM_ENABLE
	#define	A_HAS_TIMER8			1
#endif // #ifdef DDC_SYSTEM_ENABLE


#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))
#define XMODEM_DATA_AREA	__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))
#define ETH_DATA_AREA		__attribute__((section(".d2ram")))   	 __attribute__ ((aligned (32)))

//#define	A_HAS_UART1			1
//#define	A_HAS_UART2			1
#define	A_HAS_UART3				1
//#define	A_HAS_UART4			1
//#define	A_HAS_UART5			1
//#define	A_HAS_UART6			1
//#define	A_HAS_UART7			1
#if defined(A_HAS_UART1) || defined(A_HAS_UART2) || defined(A_HAS_UART3) || defined(A_HAS_UART4) || defined(A_HAS_UART5) || defined(A_HAS_UART6) || defined(A_HAS_UART7)
	#define	A_HAS_UARTS			1
#endif

//#define	A_HAS_SPI1				1
//#define	A_HAS_SPI2				1
//#define	A_HAS_SPI3				1
//#define	A_HAS_SPI4				1
#if defined(A_HAS_SPI1) || defined(A_HAS_SPI2) || defined(A_HAS_SPI3) || defined(A_HAS_SPI4)
	#define	A_HAS_SPI_BUS				1
#endif

//#define	A_HAS_I2C1				1
//#define	A_HAS_I2C2				1
//#define	A_HAS_I2C3				1
//#define	A_HAS_I2C4				1
#if defined(A_HAS_I2C1) || defined(A_HAS_I2C2) || defined(A_HAS_I2C3) || defined(A_HAS_I2C4)
	#define	A_HAS_I2C_BUS				1
#endif

//#define	A_HAS_TIMER1			1
//#define	A_HAS_TIMER2			1
//#define	A_HAS_TIMER3			1
//#define	A_HAS_TIMER4			1
//#define	A_HAS_TIMER5			1
//#define	A_HAS_TIMER6			1
//#define	A_HAS_TIMER7			1
//#define	A_HAS_TIMER8			1
//#define	A_HAS_TIMER9			1
//#define	A_HAS_TIMER10			1
//#define	A_HAS_TIMER11			1
//#define	A_HAS_TIMER12			1
//#define	A_HAS_TIMER13			1
//#define	A_HAS_TIMER14			1
//#define	A_HAS_TIMER15			1
//#define	A_HAS_TIMER16			1
#if defined	(A_HAS_TIMER1) || (A_HAS_TIMER2) || (A_HAS_TIMER3) || (A_HAS_TIMER4) || (A_HAS_TIMER5) || (A_HAS_TIMER6) \
				|| (A_HAS_TIMER7) || (A_HAS_TIMER8) || (A_HAS_TIMER9) || (A_HAS_TIMER10) || (A_HAS_TIMER11) || (A_HAS_TIMER12) \
				|| (A_HAS_TIMER13) || (A_HAS_TIMER14) || (A_HAS_TIMER15) || (A_HAS_TIMER16)
	#define	A_HAS_TIMERS				1
#endif


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

#define	BOARD_NAME			"STM32H753ZI_NUCLEO_MOTOR_SHIELD"
#define	MACHINE_NAME		"STM32H753ZI_NUCLEO_MOTOR_SHIELD"
#define	MACHINE_VERSION		"A"


#endif /* BOARDS_STM32H753ZI_NUCLEO_MOTOR_SHIELD_H_ */
