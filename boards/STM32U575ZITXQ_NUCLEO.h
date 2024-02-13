/*
 * STM32U575ZITXQ_NUCLEO.h
 *
 *  Created on: Feb 13, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32U575ZITXQ_NUCLEO_H_
#define BOARDS_STM32U575ZITXQ_NUCLEO_H_

/* Memories */

// memory pool defines
extern	uint8_t					*_mempool_start,*_mempool_end;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    256
#define	POOL_SIZE			    (&_mempool_end - &_mempool_start)
// POOL_NUM must be a constant value to compile
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
#define HSI_CLOCK         		80000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	ASSIGNED				1

extern	UART_HandleTypeDef 		huart2;
#define	CONSOLE					huart2
#define	USB_ENABLED				1

#ifdef USB_ENABLED
	#define	USB_CDC				1
#endif // #ifdef USB_ENABLED

/*
#define DMA_NOCACHE_RAM		__attribute__((section(".dmaNoCache")))   __attribute__ ((aligned (32)))
*/
#define	BOARD_NAME			"STM32U575ZITXQ_NUCLEO"
#define	MACHINE_NAME		"STM32U575ZITXQ_NUCLEO"
#define	MACHINE_VERSION		"A"

#endif /* BOARDS_STM32U575ZITXQ_NUCLEO_H_ */
