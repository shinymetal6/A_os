/*
 * SENSORS_L431.h
 *
 *  Created on: Oct 26, 2024
 *      Author: fil
 */

#ifndef BOARDS_SENSORS_L431_H_
#define BOARDS_SENSORS_L431_H_

/* Memories */

// memory pool defines
extern	uint8_t					*_mempool_start,*_mempool_end;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    256
#define	POOL_SIZE			    (POOL_END - POOL_START)
// POOL_NUM must be a constant value to compile
#define	POOL_NUM			    32
//#define POOL_ENABLE				1

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
#define	ITCM_AREA_CODE


/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		48000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

#define	BOARD_NAME			"SENSORS_L431"
#define	MACHINE_NAME		"Internal"
#define	MACHINE_VERSION		"A"

#endif /* BOARDS_SENSORS_L431_H_ */
