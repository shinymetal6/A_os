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
 * AX_P2416176_00_NEVOL.h
 *
 *  Created on: Jul 3, 2024
 *      Author: fil
 */

#ifndef BOARDS_AX_P2416176_00_NEVOL_H_
#define BOARDS_AX_P2416176_00_NEVOL_H_

/* Memories */
// memory pool defines
extern	uint8_t					*_mempool_start,*_mempool_end;
extern	uint32_t				_mempool_size;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    256
#define	POOL_SIZE			    (&_mempool_end - &_mempool_start)
// POOL_NUM must be a constant value to compile,
#define	POOL_NUM			    (0x4000 / POOL_CHUNK_SIZE)

// system defines
extern	uint8_t					*_osSysRam_start,*_osSysRam_end;
#define SRAM_START				(uint32_t )(&_osSysRam_start)
#define SRAM_END				(uint32_t )(&_osSysRam_end)
#define SRAM_SIZE				(&_osSysRam_end - &_osSysRam_start)
#define SIZE_SCHED_STACK		4096U
#define SCHED_STACK_START		SRAM_END

#define IDLE_STACK_START		(uint32_t )(&_osSysRam_end - (uint32_t )SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          4096U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		170000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */

#define	INTERNAL_ADC_ENABLED				1
#ifdef INTERNAL_ADC_ENABLED
	extern	ADC_HandleTypeDef 	hadc2;
	#define	ADC_HANDLE			hadc2
	#define	ADC_CHANNEL			3
	extern	TIM_HandleTypeDef 	htim6;
	#define	ADC_TIMER			htim6
	#define	ADC_SINGLE_CHANNEL	1
	#ifdef ADC_SINGLE_CHANNEL
		#define	ADC_SINGLE_CHANNEL_NUMBER	0
	#endif // #ifdef ADC_SINGLE_CHANNEL
#endif // #ifdef INTERNAL_ADC_ENABLED

#define	INTERNAL_DAC_ENABLED				1
#ifdef INTERNAL_DAC_ENABLED
	extern	DAC_HandleTypeDef 	hdac1;
	#define	DAC_HANDLE			hdac1
	extern	TIM_HandleTypeDef 	htim7;
	#define	DAC_TIMER			htim7
#endif // #ifdef INTERNAL_DAC_ENABLED

#define	USB_ENABLED				1
#ifdef USB_ENABLED
	#define	USB_CDC				1
#endif // #ifdef USB_ENABLED

//#define	INTERNAL_RTC_ENABLED				1
#ifdef INTERNAL_RTC_ENABLED
	extern	RTC_HandleTypeDef hrtc;
#endif // #ifdef INTERNAL_RTC_ENABLED

#define	A_HAS_UART1			1
#define	A_HAS_UART2				1
//#define	A_HAS_UART3				1
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

#define	A_HAS_I2C1				1
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

#ifdef	A_HAS_I2C_BUS
	#define	I2CFLASH_ENABLED		1
	#ifdef I2CFLASH_ENABLED
		#define	I2CFLASH_ADDRESS	0xa0
		#define	I2CFLASH_DEVICE0	I2CFLASH_ADDRESS
		#define	I2CFLASH_DEVICE1	I2CFLASH_ADDRESS+0x02
		#define	I2CFLASH_DEVICE2	I2CFLASH_ADDRESS+0x04
		#define	I2CFLASH_DEVICE3	I2CFLASH_ADDRESS+0x06
		#define	I2CFLASH_DEVICE4	I2CFLASH_ADDRESS+0x08
		#define	I2CFLASH_DEVICE5	I2CFLASH_ADDRESS+0x0a
		#define	I2CFLASH_DEVICE6	I2CFLASH_ADDRESS+0x0c
		#define	I2CFLASH_DEVICE7	I2CFLASH_ADDRESS+0x0e
	#endif // #ifdef I2CFLASH_ENABLED
#endif // #ifdef A_HAS_I2C_BUS


//#define	ITCM_AREA_CODE		__attribute__((section(".RamFunc"))) __attribute__ ((aligned (32)))
#define	ITCM_AREA_CODE
#define	DTCM_VECTORS_DATA

#define	BOARD_NAME			"AX_P2416176_00_NEVOL"
#define	MACHINE_NAME		"AX_P2416176_00_NEVOL"
#define	MACHINE_VERSION		"A"
#define	ASSIGNED			1

#endif /* BOARDS_AX_P2416176_00_NEVOL_H_ */
