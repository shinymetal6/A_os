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
	#ifdef	FY_201023_00
		#include "../boards/FY_201023_00.h"
	#endif
	#ifdef	FY_201023_01
		#include "../boards/FY_201023_01.h"
	#endif
	#ifdef	BB2xx_743
		#include "../boards/BB2xx_743.h"
	#endif
	#ifdef	VB1xx
		#include "../boards/VB1xx.h"
	#endif
	#ifdef	MEMBRANE_2412172_00
		#include "../boards/MEMBRANE_2412172_00.h"
	#endif
#endif	//#ifdef	STM32H743xx


#ifdef	STM32H753xx
	#ifdef STM32H753ZI_NUCLEO
		#include "../boards/STM32H753ZI_NUCLEO.h"
	#endif
#endif

#ifdef	STM32H563xx
	#ifdef STM32H563ZI2_NUCLEO
		#include "../boards/STM32H563ZI_NUCLEO.h"
	#endif
#endif

#ifdef	STM32U575xx
	#ifdef AX_P2212157_CONC
		#include "../boards/AX_P2212157_CONC.h"
	#endif
	#ifdef STM32U575ZIT_NUCLEO
		#include "../boards/STM32U575ZITXQ_NUCLEO.h"
	#endif
	#ifdef STM32U575ZIT_NUCLEO_DCC
		#include "../boards/STM32U575ZITXQ_NUCLEO_DCC.h"
	#endif
	#ifdef STM32U575ZIT_NUCLEO_DMX
		#include "../boards/STM32U575ZITXQ_NUCLEO_DMX.h"
	#endif
#endif

#ifdef STM32F746xx
	#ifdef STM32F746_GDISCO
		#include "../boards/STM32F746G_DISCO.h"
	#endif
#endif

#ifdef STM32L431xx
	#ifdef AX_P2311171_RS
		#include "../boards/AX_P2311171_RS.h"
	#endif
#endif

#ifdef STM32F446xx
	#ifdef STM32F446RE_NUCLEO
		#include "../boards/STM32F446RE_NUCLEO.h"
	#endif
#endif

#ifdef STM32L152xE
	#ifdef STM32L152RE_NUCLEO
		#include "../boards/STM32L152RE_NUCLEO.h"
	#endif
#endif

#ifdef STM32WL5Mxx
		#include "../boards/STM32WL5MOC_CM0.h"
#endif

#ifdef STM32G491xx
	#ifdef NEVOL_2416176
		#include "../boards/AX_P2416176_NEVOL.h"
	#endif
	#ifdef MEMBRANE_2412171_00
		#include "../boards/MEMBRANE_2412171_00.h"
	#endif
#endif
#ifndef ASSIGNED
#error "Processor not implemented"
#endif

/* common for all processors */


#define SYSTEM_RAM				__attribute__((section(".osSysRamStart"))) __attribute__ ((aligned (32)))
#define POOL_RAM				__attribute__((section(".osMemPoolStart"))) __attribute__ ((aligned (32)))
#define VERSIONING				__attribute__((section(".aos_nver"))) __attribute__ ((aligned (32))) const
#define AOS_FLASH_END			__attribute__((section(".aos_flash_end"))) __attribute__ ((aligned (32))) const
//#define	VERSIONING
// interrupts priorities
#define	PendSV_PRIORITY			15
#define	SysTick_PRIORITY		12
#define	Exti0_PRIORITY			13
#define	Exti1_PRIORITY			13
#define	Exti2_PRIORITY			13
// versioning
#define	A_OS_VERSION			"v2024.05.00-rc3"

#endif /* KERNEL_SYSTEM_DEFAULT_H_ */
