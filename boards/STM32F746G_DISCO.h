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
 * STM32F746G_DISCO.h
 *
 *  Created on: Feb 26, 2024
 *      Author: fil
 */

#ifndef BOARDS_STM32F746G_DISCO_H_
#define BOARDS_STM32F746G_DISCO_H_

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
/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		200000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK


extern	UART_HandleTypeDef 	huart1;
#define	CONSOLE				huart1

#define	CODEC_ENABLED			1
#define	CODEC_WM8994			1

#ifdef CODEC_ENABLED
	#define AUDIO_BUF_SIZE 			1024
	#ifdef CODEC_WM8994
		//#define SYNTH_ENGINE_ENABLE	1
		#define CODEC_WM8994_ADDR		(0x1a << 1)
		#define CODEC_WM8994_TIMEOUT	(1000)
		extern	I2C_HandleTypeDef 		hi2c1;
		#define	CODEC_WM8994_I2C		hi2c1
		/*
		extern	I2S_HandleTypeDef 	hi2s2;
		#define	CODEC_WM8994_I2S		hi2s2
		*/
		#define	EXT_FREQ_12_288MHZ	1
		#define	SAMPLE_FREQUENCY				48000
		#define	SAMPLE_PERIOD					(1.0F / SAMPLE_FREQUENCY)
	#endif // #ifdef CODEC_WM8994
#endif // #ifdef CODEC_ENABLED

/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

#define	BOARD_NAME			"STM32H743ZI2_NUCLEO"


#endif /* BOARDS_STM32F746G_DISCO_H_ */
