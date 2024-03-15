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
 * AX_P2311171_RS.h
 *
 *  Created on: Feb 13, 2024
 *      Author: fil
 */

#ifndef BOARDS_AX_P2311171_RS_H_
#define BOARDS_AX_P2311171_RS_H_

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

#define IDLE_STACK_START		(uint32_t )(&_osSysRam_end - SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          4096U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)

/* I/O */
#define	SENSPOWER_GPIOPORT				SENSORS_POWER_GPIO_Port
#define	SENSPOWER_GPIOBIT				SENSORS_POWER_Pin
#define	ST25POWER_GPIOPORT				ST25_POWER_GPIO_Port
#define	ST25POWER_GPIOBIT				ST25_POWER_Pin
#define	ST25GPO_GPIOPORT				GPO_GPIO_Port
#define	ST25GPO_GPIOBIT					GPO_Pin

/*
#define	MODBUS_ENABLED				1
#define	XMODEM_ENABLED				1
#define	DDC_SYSTEM_ENABLE			1
*/
#define	SENSORS_LIS2DW12			1
#define	SENSORS_LPS22DF				1
#define	SENSORS_SHT40				1
#define	SENSORS_STTS22H				1
#define	WIRELESS_ST25DVXX			1

//#define	A_HAS_UART1			1
//#define	A_HAS_UART2			1
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
#if defined	(A_HAS_TIMER1) || (A_HAS_TIMER2) || (A_HAS_TIMER3) || (A_HAS_TIMER4) || (A_HAS_TIMER5) || (A_HAS_TIMER6) \
				|| (A_HAS_TIMER7) || (A_HAS_TIMER8) || (A_HAS_TIMER9) || (A_HAS_TIMER10) || (A_HAS_TIMER11) || (A_HAS_TIMER12) \
				|| (A_HAS_TIMER13) || (A_HAS_TIMER14) || (A_HAS_TIMER15) || (A_HAS_TIMER16)
	#define	A_HAS_TIMERS				1
#endif

#ifdef SENSORS_LIS2DW12
#define	SENSORS_LIS2DW12_I2C_INDEX	HW_I2C1
#endif

#ifdef SENSORS_LPS22DF
#define	SENSORS_LPS22DF_I2C_INDEX	HW_I2C1
#endif

#ifdef SENSORS_SHT40
#define	SENSORS_SHT40_I2C_INDEX		HW_I2C1
#endif

#ifdef SENSORS_STTS22H
#define	SENSORS_STTS22H_I2C_INDEX	HW_I2C1
#endif

#ifdef WIRELESS_ST25DVXX
extern	I2C_HandleTypeDef 			hi2c1;
#define	WIRELESS_ST25DVXX_I2C_BUS	hi2c1
#endif

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		48000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	ASSIGNED				1

#define	ITCM_AREA_CODE

#define	BOARD_NAME			"AX_P2311171_RS"
#define	MACHINE_NAME		"Sensors"
#define	MACHINE_VERSION		"A"

#endif /* BOARDS_AX_P2311171_RS_H_ */
