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
 * STM32N657xx.h
 *
 *  Created on: Oct 29, 2025
 *      Author: fil
 */

#ifndef PROCESSORS_STM32N657XX_H_
#define PROCESSORS_STM32N657XX_H_

#include "iodef_common.h"
#include "memdef_common.h"

#define DTCM_VECTORS_DATA
#define DWNLD_DATA_AREA
#define ETH_DATA_AREA
#define FRAME_BUFFER

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		600000000U
#define SYSTICK_TIM_CLK   		75000000

#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12

#endif /* PROCESSORS_STM32N657XX_H_ */
