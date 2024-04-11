/*
 * STM32WL5MOC_CM0.h
 *
 *  Created on: Apr 10, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32WL5MOC_CM0_H_
#define BOARDS_STM32WL5MOC_CM0_H_

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
#define HSI_CLOCK         		32000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */

//#define	USB_ENABLED				1
#ifdef USB_ENABLED
	#define	USB_CDC				1
#endif // #ifdef USB_ENABLED

//#define	DDC_SYSTEM_ENABLE		1
#define	XMODEM_ENABLE				1
#define	MODBUS_ENABLE				1


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

#define	ITCM_AREA_CODE

#define	BOARD_NAME			"STM32WL5MOC_CM0"
#define	MACHINE_NAME		"STM32WL5MOC_CM0"
#define	MACHINE_VERSION		"A"
#define	ASSIGNED			1

#endif /* BOARDS_STM32WL5MOC_CM0_H_ */
