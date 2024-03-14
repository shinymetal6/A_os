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
 * AX_P2212157_CONC.h
 *
 *  Created on: Feb 13, 2024
 *      Author: fil
 */

#ifndef BOARDS_AX_P2212157_CONC_H_
#define BOARDS_AX_P2212157_CONC_H_

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

// custom defines
#define CUSTOM_RAM		__attribute__((section(".customRam_start")))	__attribute__ ((aligned (32)))
#define CUSTOM_RAM_END	__attribute__((section(".customRam_end")))		__attribute__ ((aligned (32)))

extern	uint8_t					*_customRam_start,*_customRam_end;
#define CUSTOM_RAM__START		(uint32_t )(&_customRam_start)
#define CUSTOM_RAM__END			(uint32_t )(&_customRam_end)
#define CUSTOM_RAM__SIZE		(&_customRam_end - &_customRam_start)

/* I/O */
#define	HEARTBEAT_LED_GPIOPORT			HEARTBEAT_LED_GPIO_Port
#define	HEARTBEAT_LED_GPIOBIT			HEARTBEAT_LED_Pin
#define	UPFAIL_LED_GPIOPORT				UPFAIL_LED_GPIO_Port
#define	UPFAIL_LED_GPIOBIT				UPFAIL_LED_Pin
#define	PROTO_FAIL_LED_GPIOPORT			PROTO_FAIL_LED_GPIO_Port
#define	PROTO_FAIL_LED_GPIOBIT			PROTO_FAIL_LED_Pin
#define	THIS_BOARD_MASTER_N_GPIOPORT	THIS_BOARD_MASTER_N_GPIO_Port
#define	THIS_BOARD_MASTER_N_GPIOBIT		THIS_BOARD_MASTER_N_Pin
#define	SET_SLAVE_PRESENT_GPIOPORT		SET_SLAVE_PRESENT_GPIO_Port
#define	SET_SLAVE_PRESENT_GPIOBIT		SET_SLAVE_PRESENT_Pin
#define	SLAVE_ENABLE_N_GPIOPORT			SLAVE_ENABLE_N_GPIO_Port
#define	SLAVE_ENABLE_N_GPIOBIT			SLAVE_ENABLE_N_Pin
#define	PW_N_GPIOPORT					PW_N_GPIO_Port
#define	PW_N_GPIOBIT					PW_N_Pin
#define	SEL0_GPIOPORT					SEL0_GPIO_Port
#define	SEL0_GPIOBIT					SEL0_Pin
#define	SEL1_GPIOPORT					SEL1_GPIO_Port
#define	SEL1_GPIOBIT					SEL1_Pin
#define	FAIL0_GPIOPORT					FAIL0_GPIO_Port
#define	FAIL0_GPIOBIT					FAIL0_Pin
#define	FAIL1_GPIOPORT					FAIL1_GPIO_Port
#define	FAIL1_GPIOBIT					FAIL1_Pin
#define	FAIL2_GPIOPORT					FAIL2_GPIO_Port
#define	FAIL2_GPIOBIT					FAIL2_Pin
#define	FAIL3_GPIOPORT					FAIL3_GPIO_Port
#define	FAIL3_GPIOBIT					FAIL3_Pin
#define	RS485RE0_GPIOPORT				RS485RE0_GPIO_Port
#define	RS485RE0_GPIOBIT				RS485RE0_Pin
#define	RS485RE1_GPIOPORT				RS485RE1_GPIO_Port
#define	RS485RE1_GPIOBIT				RS485RE1_Pin
#define	RS485RE2_GPIOPORT				RS485RE2_GPIO_Port
#define	RS485RE2_GPIOBIT				RS485RE2_Pin
#define	RS485RE3_GPIOPORT				RS485RE3_GPIO_Port
#define	RS485RE3_GPIOBIT				RS485RE3_Pin

/* Clock */
#define TICK_HZ 				1000U
//#define HSI_CLOCK         		160000000U
#define HSI_CLOCK         		80000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK

/* Chip peripherals utilization */

#define	USB_ENABLED				1
#ifdef USB_ENABLED
	#define	USB_CDC				1
#endif // #ifdef USB_ENABLED

#define	A_HAS_UART1			1
#define	A_HAS_UART2			1
#define	A_HAS_UART3				1
//#define	A_HAS_UART4			1
//#define	A_HAS_UART5			1
//#define	A_HAS_UART6			1
#define	A_HAS_UART7			1
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

// Board configuration

#define	SENSOR_USART			HW_UART2

#define	SELECT_SENSORS_0		0
#define	SELECT_SENSORS_1		1
#define	SELECT_SENSORS_2		2
#define	SELECT_SENSORS_3		3

#define	ITCM_AREA_CODE

#define	BOARD_NAME			"AX_P2212157"
#define	MACHINE_NAME		"Conc"
#define	MACHINE_VERSION		"A"
#define	ASSIGNED				1

#endif /* BOARDS_AX_P2212157_CONC_H_ */
