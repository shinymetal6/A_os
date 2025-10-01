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

#include <stdint.h>

#ifdef	STM32H743xx
		#include "../processors/STM32H743xx.h"
#endif
#ifdef	STM32H753xx
		#include "../processors/STM32H753xx.h"
#endif
#ifdef	STM32F722xx
		#include "../processors/STM32F722xx.h"
#endif
#ifdef	STM32G474xx
		#include "../processors/STM32G474xx.h"
#endif
#ifdef	STM32U575xx
		#include "../processors/STM32U575xx.h"
#endif
#ifdef	STM32F446xx
		#include "../processors/STM32F446xx.h"
#endif
#ifdef	STM32L152xE
		#include "../processors/STM32L152xE.h"
#endif

#if __has_include("user_config.h") && __has_include(<stdint.h>)
	#include "user_config.h"
	#define		USER_PROCESSES	1
	#undef		SAMPLE_PROCESSES_ENABLED
#else
	#include "../SampleProcesses/sample_user_config.h"
	#define		SAMPLE_PROCESSES_ENABLED		1
	#undef		USER_PROCESSES
	//#warning 	"Sample processes enabled"
#endif

// versioning
#define	A_OS_VERSION			"v2025.11.00_rc1"
#endif /* KERNEL_SYSTEM_DEFAULT_H_ */
