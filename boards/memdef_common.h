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
 * memdef_common.h
 *
 *  Created on: Nov 16, 2024
 *      Author: fil
 */

#ifndef BOARDS_MEMDEF_COMMON_H_
#define BOARDS_MEMDEF_COMMON_H_

/* Memories */

// memory pool defines
//#define POOL_ENABLE				1
#ifdef POOL_ENABLE
extern	uint8_t					*_mempool_start,*_mempool_end;
#define	POOL_START			    (uint32_t )(&_mempool_start)
#define	POOL_END			    (uint32_t )(&_mempool_end)
#define	POOL_CHUNK_SIZE		    256
#define	POOL_SIZE			    (POOL_END - POOL_START)
// POOL_NUM must be a constant value to compile
#define	POOL_NUM			    32
#endif

// system defines
extern	uint8_t					*_osSysRam_start,*_osSysRam_end;
#define SRAM_START				(uint32_t )(&_osSysRam_start)
#define SRAM_END				(uint32_t )(&_osSysRam_end)
#define SRAM_SIZE				(&_osSysRam_end - &_osSysRam_start)
#define SIZE_SCHED_STACK		4096U
#define SCHED_STACK_START		SRAM_END

#define IDLE_STACK_START		(uint32_t )((uint32_t )&_osSysRam_end - (uint32_t )SIZE_SCHED_STACK)
#define SIZE_IDLE_STACK          4096U
#define	FIRST_PRC_STACK_START	 (IDLE_STACK_START - SIZE_IDLE_STACK)

#define SYSTEM_RAM				__attribute__((section(".osSysRamStart"))) __attribute__ ((aligned (32)))
#define POOL_RAM				__attribute__((section(".osMemPoolStart"))) __attribute__ ((aligned (32)))
#define VERSIONING				__attribute__((section(".aos_nver"))) __attribute__ ((aligned (32))) const
#define AOS_FLASH_END			__attribute__((section(".aos_flash_end"))) __attribute__ ((aligned (32))) const
#define	ITCM_AREA_CODE			__attribute__((section(".RamITCMFunc"))) __attribute__ ((aligned (32)))

#endif /* BOARDS_MEMDEF_COMMON_H_ */
