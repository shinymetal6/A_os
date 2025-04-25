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
 * system_default.h
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#ifndef KERNEL_SYSTEM_DEFAULT_H_
#define KERNEL_SYSTEM_DEFAULT_H_

#ifdef	STM32H743xx
	#ifdef STM32H743ZI2_NUCLEO
		#include "../boards/STM32H743ZI2_NUCLEO.h"
	#endif
	#ifdef	VB_10x
		#include "../boards/VB_10x_00.h"
	#endif
	#ifdef	MEMBRANE_2412172_00
		#include "../boards/AX_P2412172_00_MembraneConcentrator.h"
	#endif
#ifdef	NEVOL_2416176_01
		#include "../boards/AX_P2416176_01_NEVOL.h"
	#endif
#ifdef	NEVOL_AUDIO_2416176_01
		#include "../boards/AX_P2416176_01_NEVOL_AUDIO.h"
	#endif
#ifdef	FY_201023_01
		#include "../boards/FY_201023_01.h"
	#endif
#endif	//#ifdef	STM32H743xx


#ifdef	STM32H753xx
	#ifdef STM32H753ZI_NUCLEO
		#include "../boards/STM32H753ZI_NUCLEO.h"
	#endif
#endif

#ifdef	STM32L431xx
	#ifdef ROLLON_AX_P2311171
		#include "../boards/ROLLON_AX_P2311171.h"
	#endif
#endif

#ifdef	STM32G474xx
	#ifdef ARROW_AX_P2214177
		#include "../boards/ARROW_AX_P2214177.h"
	#endif
#endif

#ifdef	STM32G491xx
	#ifdef DCC_491_48
		#include "../boards/DCC_491_48.h"
	#endif
	#ifdef NUCLEO_G491
		#include "../boards/NUCLEO_G491.h"
	#endif
	#ifdef MEMBRANE_WS_2412171_00
		#include "../boards/MEMBRANE_WS_2412171_00.h"
	#endif
#endif

#ifndef ASSIGNED
#error "Processor not implemented"
#endif

/* common for all processors */

// interrupts priorities
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	Exti0_PRIORITY			13
#define	Exti1_PRIORITY			13
#define	Exti2_PRIORITY			13
// versioning
#define	A_OS_VERSION			"v2025.06.00-rc0"

#endif /* KERNEL_SYSTEM_DEFAULT_H_ */
