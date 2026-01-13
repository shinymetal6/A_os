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
 * STM32F446xx.h
 *
 *  Created on: Sep 30, 2025
 *      Author: fil
 */

#ifndef PROCESSORS_STM32F446XX_H_
#define PROCESSORS_STM32F446XX_H_

#include "iodef_common.h"
#include "memdef_common.h"

#undef	HAS_BRR

#define DTCM_VECTORS_DATA
#define DWNLD_DATA_AREA
#define ETH_DATA_AREA
#define FRAME_BUFFER

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		180000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
#define	DFU_BOOT_ENTRY			0x1FF09800
#define	DFU_BOOT_VERSION_PTR	0x1FF1E7FE
#define	DFU_BOOT_VERSION		0x91

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	Exti0_PRIORITY			13
#define	Exti1_PRIORITY			13
#define	Exti2_PRIORITY			13

#endif /* PROCESSORS_STM32F446XX_H_ */
