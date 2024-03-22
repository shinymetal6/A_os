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
 * STM32L152RE_NUCLEO.h
 *
 *  Created on: Mar 22, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32L152RE_NUCLEO_H_
#define BOARDS_STM32L152RE_NUCLEO_H_

/* Memories */

// memory pool defines
extern	uint8_t					*_mempool_start,*_mempool_end;
extern	uint32_t				_mempool_size;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    256
#define	POOL_SIZE			    (&_mempool_end - &_mempool_start)
// POOL_NUM must be a constant value to compile,
#define	POOL_NUM			    16

// system defines
extern	uint8_t					*_osSysRam_start,*_osSysRam_end;
#define SRAM_START				(uint32_t )(&_osSysRam_start)
#define SRAM_END				(uint32_t )(&_osSysRam_end)
#define SRAM_SIZE				(&_osSysRam_end - &_osSysRam_start)
#define SIZE_SCHED_STACK		4096U
#define SCHED_STACK_START		SRAM_END

#define IDLE_STACK_START		(uint32_t )(&_osSysRam_end - SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          4096U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)

#define	LED_2_GPIOPORT			LD2_GPIO_Port
#define	LED_2_GPIOBIT			LD2_Pin
#define	BUTTON_GPIOPORT			B1_GPIO_Port
#define	BUTTON_GPIOBIT			B1_Pin


/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		32000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */

#define	LED_2_GPIOPORT			LD2_GPIO_Port
#define	LED_2_GPIOBIT			LD2_Pin
#define	BUTTON_GPIOPORT			B1_GPIO_Port
#define	BUTTON_GPIOBIT			B1_Pin

//#define	DDC_SYSTEM_ENABLE		1
#define	XMODEM_ENABLE				1
#define	MODBUS_ENABLE				1
#define	WIRELESS_ENABLED			1
#define	WIRELESS_NRF24L01			1

#ifdef WIRELESS_ENABLED
	#ifdef WIRELESS_NRF24L01
		extern	SPI_HandleTypeDef 					hspi2;
		#define NRF24L01_SPI						hspi2
		#define NRF24L01_SPI_TIMEOUT				2000

		#define NRF24L01_SS_PIN_PORT				NRF24L01_SS_GPIO_Port
		#define NRF24L01_SS_PIN_NUMBER				NRF24L01_SS_Pin

		#define NRF24L01_CE_PIN_PORT				NRF24L01_CE_GPIO_Port
		#define NRF24L01_CE_PIN_NUMBER				NRF24L01_CE_Pin

		#define NRF24L01_IRQ_PIN_PORT				NRF24L01_IRQ_GPIO_Port
		#define NRF24L01_IRQ_PIN_NUMBER				NRF24L01_IRQ_Pin
	#endif // #ifdef WIRELESS_NRF24L01
#endif // #ifdef WIRELESS_ENABLED


//#define	A_HAS_UART1			1
#define	A_HAS_UART2			1
//#define	A_HAS_UART3			1
//#define	A_HAS_UART4			1
//#define	A_HAS_UART5			1
//#define	A_HAS_UART6			1
//#define	A_HAS_UART7			1
#if defined(A_HAS_UART1) || defined(A_HAS_UART2) || defined(A_HAS_UART3) || defined(A_HAS_UART4) || defined(A_HAS_UART5) || defined(A_HAS_UART6) || defined(A_HAS_UART7)
	#define	A_HAS_UARTS			1
#endif

//#define	A_HAS_SPI1				1
#define	A_HAS_SPI2				1
//#define	A_HAS_SPI3				1
//#define	A_HAS_SPI4				1
#if defined(A_HAS_SPI1) || defined(A_HAS_SPI2) || defined(A_HAS_SPI3) || defined(A_HAS_SPI4)
	#define	A_HAS_SPI_BUS				1
#endif

//#define	A_HAS_I2C1				1
//#define	A_HAS_I2C2				1
#if defined(A_HAS_I2C1) || defined(A_HAS_I2C2)
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

#define	ITCM_AREA_CODE

#define	BOARD_NAME			"STM32L152RE_NUCLEO"
#define	MACHINE_NAME		"STM32L152RE_NUCLEO"
#define	MACHINE_VERSION		"A"
#define	ASSIGNED			1
#endif /* BOARDS_STM32L152RE_NUCLEO_H_ */