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
 * STM32H743ZI2_NUCLEO.h
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32H743ZI2_NUCLEO_H_
#define BOARDS_STM32H743ZI2_NUCLEO_H_

/* Memories */

// memory pool defines
extern	uint8_t					*_mempool_start,*_mempool_end;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    1024
#define	POOL_SIZE			    (POOL_END - POOL_START)
// POOL_NUM must be a constant value to compile
#define	POOL_NUM			    84

// system defines
extern	uint8_t					*_osSysRam_start,*_osSysRam_end;
#define SRAM_START				(uint32_t )(&_osSysRam_start)
#define SRAM_END				(uint32_t )(&_osSysRam_end)
#define SRAM_SIZE				(&_osSysRam_end - &_osSysRam_start)
#define SIZE_SCHED_STACK		2048U
#define SCHED_STACK_START		SRAM_END

#define IDLE_STACK_START		(uint32_t )((uint32_t )&_osSysRam_end - (uint32_t )SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          2048U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)


/* I/O */
#define	DEBUG_GPIOPORT			PG6_Debug_GPIO_Port
#define	DEBUG_GPIOBIT			PG6_Debug_Pin
#define	LED_1_GPIOPORT			LD1_GPIO_Port
#define	LED_1_GPIOBIT			LD1_Pin
#define	LED_2_GPIOPORT			LD2_GPIO_Port
#define	LED_2_GPIOBIT			LD2_Pin
#define	LED_3_GPIOPORT			LD3_GPIO_Port
#define	LED_3_GPIOBIT			LD3_Pin
#define	BUTTON_GPIOPORT			B1_GPIO_Port
#define	BUTTON_GPIOBIT			B1_Pin
#define	ST25R3916B				1

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		480000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart3;
#define	CONSOLE				huart3

#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))

#define	BOARD_NAME			"STM32H743ZI2_NUCLEO"

#endif /* BOARDS_STM32H743ZI2_NUCLEO_H_ */
