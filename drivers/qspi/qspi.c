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
 * qspi.c
 *
 *  Created on: Jan 8, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

SYSTEM_RAM	ExtFlash_DriverStruct_t	ExtFlashDriverStruct[MAX_EXTMEM_DRIVERS];
SYSTEM_RAM	uint8_t					last_qspi_used_handle=0;

uint32_t qspi_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
	if ( ExtFlashDriverStruct[handle].read != NULL )
		return ExtFlashDriverStruct[handle].read(handle,address,data,data_len);
	return 1;
}

uint32_t qspi_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
	if ( ExtFlashDriverStruct[handle].write != NULL )
		return ExtFlashDriverStruct[handle].write(handle,address,data,data_len);
	return 1;
}

uint32_t qspi_erase_blocks(uint8_t handle, uint32_t start_block,uint32_t number_of_blocks)
{
	if ( ExtFlashDriverStruct[handle].erase_blocks != NULL )
		return ExtFlashDriverStruct[handle].erase_blocks(handle,start_block,number_of_blocks);
	return 1;
}

uint32_t qspi_erase_chip(uint8_t handle)
{
	if ( ExtFlashDriverStruct[handle].erase_chip != NULL )
		return ExtFlashDriverStruct[handle].erase_chip(handle);
	return 1;
}

uint32_t qspi_get_id(uint8_t handle,uint8_t *data)
{
	if ( ExtFlashDriverStruct[handle].get_id != NULL )
		return ExtFlashDriverStruct[handle].get_id(handle,data);
	return 1;
}

uint32_t qspi_get_status(uint8_t handle)
{
	if ( ExtFlashDriverStruct[handle].get_status != NULL )
		return ExtFlashDriverStruct[handle].get_status(handle);
	return 1;
}

uint32_t qspi_get_flags(uint8_t handle)
{
	if ( ExtFlashDriverStruct[handle].get_flags != NULL )
		return ExtFlashDriverStruct[handle].get_flags(handle);
	return 1;
}

uint32_t qspi_set_flags(uint8_t handle, uint32_t flags)
{
	if ( ExtFlashDriverStruct[handle].set_flags != NULL )
		return ExtFlashDriverStruct[handle].set_flags(handle,flags);
	return 1;
}


