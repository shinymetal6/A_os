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
 * STM32H563ZI_NUCLEO.h
 *
 *  Created on: Mar 1, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32H563ZI_NUCLEO_H_
#define BOARDS_STM32H563ZI_NUCLEO_H_

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
#define	LED_1_GPIOPORT			LED3_RED_GPIO_Port
#define	LED_1_GPIOBIT			LED3_RED_Pin
#define	LED_2_GPIOPORT			LED1_GREEN_GPIO_Port
#define	LED_2_GPIOBIT			LED1_GREEN_Pin
#define	LED_3_GPIOPORT			LED2_YELLOW_GPIO_Port
#define	LED_3_GPIOBIT			LED2_YELLOW_Pin
#define	BUTTON_GPIOPORT			USER_BUTTON_GPIO_Port
#define	BUTTON_GPIOBIT			USER_BUTTON_Pin

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		250000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12

//#define	USB_ENABLED				1
#ifdef USB_DEVICE_ENABLED
	#define	USB_CDC				1
#endif // #ifdef USB_ENABLED

//#define	DDC_SYSTEM_ENABLE		1
#define	XMODEM_ENABLE				1
#define	MODBUS_ENABLE				1


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
//#define	A_HAS_TIMER17			1
#if defined	(A_HAS_TIMER1) || (A_HAS_TIMER2) || (A_HAS_TIMER3) || (A_HAS_TIMER4) || (A_HAS_TIMER5) || (A_HAS_TIMER6) \
				|| (A_HAS_TIMER7) || (A_HAS_TIMER8) || (A_HAS_TIMER9) || (A_HAS_TIMER10) || (A_HAS_TIMER11) || (A_HAS_TIMER12) \
				|| (A_HAS_TIMER13) || (A_HAS_TIMER14) || (A_HAS_TIMER15) || (A_HAS_TIMER16) || (A_HAS_TIMER17)
	#define	A_HAS_TIMERS				1
#endif


/*
#define DMA_NOCACHE_RAM		__attribute__((section(".dmaNoCache")))   __attribute__ ((aligned (32)))
*/

//#define	ITCM_AREA_CODE		__attribute__((section(".RamFunc"))) 	__attribute__ ((aligned (32)))
#define	ITCM_AREA_CODE

#define	DFU_BOOT_ENTRY			0x0BF97000
#define	DFU_BOOT_VERSION_PTR	0x0BF9FAFE
#define	DFU_BOOT_VERSION		0xE4

#define	BOARD_NAME			"STM32H563ZI_NUCLEO"
#define	MACHINE_NAME		"STM32H563ZI_NUCLEO"
#define	MACHINE_VERSION		"A"
#define	ASSIGNED			1
#endif /* BOARDS_STM32H563ZI_NUCLEO_H_ */
