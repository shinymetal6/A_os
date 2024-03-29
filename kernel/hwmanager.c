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
 * hwmanager.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "scheduler.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	Asys_t			Asys;

uint32_t allocate_hw(uint32_t peripheral,uint8_t config)
{
	if ( HWMngr[peripheral].process )
		return 0;
	HWMngr[peripheral].process = Asys.current_process;
	HWMngr[peripheral].status = HWMAN_STATUS_ALLOCATED | config;
	return peripheral;
}

uint32_t deallocate_hw(uint32_t peripheral)
{
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	HWMngr[peripheral].process = 0;
	HWMngr[peripheral].status = HWMAN_STATUS_FREE;
	return peripheral;
}

uint16_t get_rx_len(uint32_t peripheral)
{
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	return HWMngr[HW_USB_DEVICE].rxlen;
}

uint32_t get_peripheral_flags(uint32_t peripheral)
{
	if ( HWMngr[peripheral].process != Asys.current_process )
		return 0;
	return HWMngr[HW_USB_DEVICE].flags;
}

