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
 * system_default.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_SYSTEM_DEFAULT_H_
#define KERNEL_SYSTEM_DEFAULT_H_

#define	FY_201023_00	1
#ifdef	STM32H743xx
#ifdef	FY_201023_00
/* Memories */
#define	POOL_START			    0x38000000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x38000000
#define SRAM_SIZE                65536

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_1_GPIOPORT			LED_GPIO_Port
#define	LED_1_GPIOBIT			LED_Pin

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		480000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		14
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart3;
#define	CONSOLE				huart3
#define	QSPI_ENABLED			1
#define	QSPI_WINBOND			1

#define	BOARD_NAME			"FY-201023-00"
#else
/* Memories */
#define	POOL_START			    0x38000000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x38000000
#define SRAM_SIZE                65536

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_1_GPIOPORT			LD1_GPIO_Port
#define	LED_1_GPIOBIT			LD1_Pin
#define	LED_3_GPIOPORT			LD3_GPIO_Port
#define	LED_3_GPIOBIT			LD3_Pin
#define	BUTTON_GPIOPORT			B1_GPIO_Port
#define	BUTTON_GPIOBIT			B1_Pin
/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		480000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		14
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart3;
#define	CONSOLE				huart3

#define	BOARD_NAME			"STM32H743ZI2-NUCLEO"

#endif
#endif

#ifdef	STM32H563xx
/* Memories */
#define	POOL_START			    0x20070000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20080000
#define SRAM_SIZE                32768

/* I/O */

#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_1_GPIOPORT			LED3_RED_GPIO_Port
#define	LED_1_GPIOBIT			LED3_RED_Pin
#define	LED_2_GPIOPORT			LED1_GREEN_GPIO_Port
#define	LED_2_GPIOBIT			LED1_GREEN_Pin
#define	LED_3_GPIOPORT			LED2_YELLOW_GPIO_Port
#define	LED_3_GPIOBIT			LED2_YELLOW_Pin
#define	BUTTON_GPIOPORT			USER_BUTTON_GPIO_Port
#define	BUTTON_GPIOBIT			USER_BUTTON_Pin

#define TICK_HZ 				1000U
#define HSI_CLOCK         		250000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		14
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart3;
#define	CONSOLE				huart3

#define	BOARD_NAME			"STM32H563ZI-NUCLEO"

#endif


#ifdef	STM32U575xx

/* Memories */
#define	POOL_START			     0x20080000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20090000
#define SRAM_SIZE                131072

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_1_GPIOPORT			LED_RED_GPIO_Port
#define	LED_1_GPIOBIT			LED_RED_Pin
#define	LED_2_GPIOPORT			LED_GREEN_GPIO_Port
#define	LED_2_GPIOBIT			LED_GREEN_Pin
#define	LED_3_GPIOPORT			LED_BLUE_GPIO_Port
#define	LED_3_GPIOBIT			LED_BLUE_Pin
#define	BUTTON_GPIOPORT			USER_BUTTON_GPIO_Port
#define	BUTTON_GPIOBIT			USER_BUTTON_Pin

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		250000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		14
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart1;
#define	CONSOLE				huart1

#define	BOARD_NAME			"STM32U575-NUCLEO"

#endif

#ifdef	STM32F746xx
/* Memories */
#define	POOL_START			     0x20040000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20048000
#define SRAM_SIZE                32768

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin

#define TICK_HZ 				1000U
#define HSI_CLOCK         		216000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		14
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart1;
#define	CONSOLE				huart1

#define	QSPI_ENABLED			1

#define	BOARD_NAME			"STM32F746G-DISCO"
// Uncomment this out if you want a fixed ip
#undef LWIP_DHCP

#endif

#ifdef	STM32F446xx
/* Memories */
#define	POOL_START			     0x20004000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20008000
#define SRAM_SIZE                32768

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin

#define TICK_HZ 				1000U
#define HSI_CLOCK         		180000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		14
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart2;
#define	CONSOLE				huart2

#define	BOARD_NAME			"STM32F446RE-NUCLEO"
// Uncomment this out if you want a fixed ip
#undef LWIP_DHCP

#endif

#ifdef	STM32L152xE
/* Memories */
#define	POOL_START			     0x20004000
/* Note : SRAM_START must be equal to osSegment in ld file */
#define SRAM_START               0x20008000
#define SRAM_SIZE                32768

/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_GPIOPORT			LD2_LED_GPIO_Port
#define	LED_GPIOBIT				LD2_LED_Pin

#define TICK_HZ 				1000U
#define HSI_CLOCK         		180000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		14
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart2;
#define	CONSOLE				huart2

#define	BOARD_NAME			"STM32L152RE-NUCLEO"
// Uncomment this out if you want a fixed ip
#undef LWIP_DHCP

#endif

#ifndef ASSIGNED
#error "Processor not implemented"
#endif

/* common for all processors */
#define	POOL_CHUNK_SIZE		    256
#define	POOL_NUM			    32
#define	POOL_SIZE			    (POOL_CHUNK_SIZE*POOL_NUM)
#define	POOL_END			    (POOL_START+POOL_SIZE)
#define SRAM_END                 ((SRAM_START) + (SRAM_SIZE) )
#define SIZE_SCHED_STACK         4096U
#define SCHED_STACK_START        SRAM_END
#define IDLE_STACK_START         (SCHED_STACK_START - SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          4096U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)
#define SYSTEM_RAM				__attribute__((section(".osSegment"))) __attribute__ ((aligned (32)))


#endif /* KERNEL_SYSTEM_DEFAULT_H_ */
