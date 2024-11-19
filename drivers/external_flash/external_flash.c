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
 * external_flash.c
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

SYSTEM_RAM	ExtFlash_DriverStruct_t	ExtFlashDriverStruct[MAX_EXTMEM_DRIVERS];
SYSTEM_RAM	uint8_t					last_extflash_used_handle=0,extflash_driver_request = 0;

uint32_t extflash_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
	return ExtFlashDriverStruct[handle].read(handle,address,data,data_len);
}

uint32_t extflash_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
	return ExtFlashDriverStruct[handle].write(handle,address,data,data_len);
}

uint32_t extflash_erase_blocks(uint8_t handle, uint32_t start_block,uint32_t number_of_blocks)
{
	return ExtFlashDriverStruct[handle].erase_blocks(handle,start_block,number_of_blocks);
}

uint32_t extflash_erase_chip(uint8_t handle)
{
	return ExtFlashDriverStruct[handle].erase_chip(handle);
}

uint32_t extflash_get_status(uint8_t handle)
{
	return ExtFlashDriverStruct[handle].get_status(handle);
}

uint32_t extflash_get_flags(uint8_t handle)
{
	return ExtFlashDriverStruct[handle].get_flags(handle);
}

uint32_t extflash_set_flags(uint8_t handle, uint32_t flags)
{
	return ExtFlashDriverStruct[handle].set_flags(handle,flags);
}


