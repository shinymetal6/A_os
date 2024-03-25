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
 * dfu_manager.c
 *
 *  Created on: Mar 25, 2024
 *      Author: fil
 */
#include "main.h"
#include "A.h"
#include "system_default.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

void dfu_run_bootloader(void)
{
#ifdef	DFU_BOOT_ENTRY

void (*SysMemBootJump)(void);
uint8_t	id;

	volatile uint32_t addr = DFU_BOOT_ENTRY;
	volatile uint8_t *idaddr = (uint8_t *)DFU_BOOT_VERSION_PTR;

	id = *idaddr;
	if ( id == DFU_BOOT_VERSION)
	{
		HAL_RCC_DeInit();

		SysTick->CTRL = 0;
		SysTick->LOAD = 0;
		SysTick->VAL = 0;

		__disable_irq();

		SysMemBootJump = (void (*)(void)) (*((uint32_t *)(addr + 4)));
		__set_MSP(*(uint32_t *)addr);
		SysMemBootJump();
	}
#endif // #ifdef	DFU_BOOT_ENTRY
}
