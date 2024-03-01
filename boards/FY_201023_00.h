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
 * FY_201023_00.h
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */

#ifndef BOARDS_FY_201023_00_H_
#define BOARDS_FY_201023_00_H_

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

#define IDLE_STACK_START		(uint32_t )(&_osSysRam_end - SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          2048U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)

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
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

extern	UART_HandleTypeDef 	huart3;
#define	CONSOLE				huart3

#define	QSPI_ENABLED			1
#define	WIRELESS_ENABLED		1
#define	WIRELESS_NRF24L01		1
#define	LORA_ENABLED			1
#define	ETH_ENABLED				1
#define	USB_ENABLED				1
#undef 	LWIP_DHCP

#ifdef USB_ENABLED
	#define	USB_CDC				1
	//#define	USB_MIDI			1
#endif // #ifdef USB_ENABLED

#ifdef QSPI_ENABLED
	#define	QSPI_WINBOND			1
	extern	QSPI_HandleTypeDef 	hqspi;
	#define	HQSPI1				hqspi
#endif // #ifdef QSPI_ENABLED
#ifdef WIRELESS_ENABLED
	#ifdef WIRELESS_NRF24L01
		extern	SPI_HandleTypeDef 					hspi1;
		#define NRF24L01_SPI						hspi1
		#define NRF24L01_SPI_TIMEOUT				2000

		#define NRF24L01_SS_PIN_PORT				ISM_SS_GPIO_Port
		#define NRF24L01_SS_PIN_NUMBER				ISM_SS_Pin

		#define NRF24L01_CE_PIN_PORT				ISM_CE_GPIO_Port
		#define NRF24L01_CE_PIN_NUMBER				ISM_CE_Pin

		#define NRF24L01_IRQ_PIN_PORT				ISM_IRQ_GPIO_Port
		#define NRF24L01_IRQ_PIN_NUMBER				ISM_IRQ_Pin
	#endif // #ifdef WIRELESS_NRF24L01
	#ifdef LORA_ENABLED
		extern	SPI_HandleTypeDef 					hspi1;
		#define SX126X_SPI							hspi1
		#define SX126X_SPI_TIMEOUT					2000

		#define SX126X_SS_PIN_PORT					SX126x_SS_GPIO_Port
		#define SX126X_SS_PIN_NUMBER				SX126x_SS_Pin

		#define SX126X_BUSY_PIN_PORT				SX126x_BUSY_GPIO_Port
		#define SX126X_BUSY_PIN_NUMBER				SX126x_BUSY_Pin

		#define SX126X_DIO1_PIN_PORT				SX126x_DIO1_GPIO_Port
		#define SX126X_DIO1_PIN_NUMBER				SX126x_DIO1_Pin

		#define SX126X_RESET_GPIO_Port				SX126x_RESET_GPIO_Port
		#define SX126X_RESET_Pin					SX126x_RESET_Pin
	#endif // #ifdef LORA_ENABLED
#endif // #ifdef WIRELESS_ENABLED

#define	ITCM_AREA_CODE		__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))
#define DTCM_VECTORS_DATA	__attribute__((section(".dtcm_data")))   __attribute__ ((aligned (32)))


#define	BOARD_NAME			"FY-201023-00"

#endif /* BOARDS_FY_201023_00_H_ */
