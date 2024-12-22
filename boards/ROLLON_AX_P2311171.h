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
 * ROLLON_AX_P2311171.h
 *
 *  Created on: Dec 21, 2024
 *      Author: fil
 */
#ifndef BOARDS_ROLLON_AX_P2311171_H_
#define BOARDS_ROLLON_AX_P2311171_H_

#include "iodef_common.h"
#include "memdef_common.h"

#undef DTCM_VECTORS_DATA
#undef ITCM_AREA_CODE
#define DTCM_VECTORS_DATA
#define ITCM_AREA_CODE

/* Clock */
#define TICK_HZ 				1000U
#define HSI_CLOCK         		8000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK
/* Others */
/* Others */
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	ASSIGNED				1

#define	ITCM_AREA_CODE

#define	BOARD_NAME			"AX_P2311171_RS"
#define	MACHINE_NAME		"Sensors"
#define	MACHINE_VERSION		"A"

#endif /* BOARDS_ROLLON_AX_P2311171_H_ */
