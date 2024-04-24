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
 * module_manager.c
 *
 *  Created on: Apr 24, 2024
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

extern	Modules_t		Modules[MODULES_NUM];
extern	Asys_t			Asys;

ITCM_AREA_CODE uint32_t allocate_module(uint32_t module,uint8_t config)
{
	__disable_irq();
	if ( Modules[module].process )
		return 0;
	Modules[module].process = Asys.current_process;
	Modules[module].status = MODULE_STATUS_ALLOCATED | config;
	__enable_irq();
	return module;
}

ITCM_AREA_CODE uint32_t deallocate_module(uint32_t module)
{
	__disable_irq();
	if ( Modules[module].process != Asys.current_process )
		return 0;
	Modules[module].process = 0;
	Modules[module].status = MODULE_STATUS_FREE;
	__enable_irq();
	return module;
}
