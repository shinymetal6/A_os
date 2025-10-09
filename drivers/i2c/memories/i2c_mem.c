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
 * i2c_mem.c
 *
 *  Created on: Jan 8, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_I2C_ENABLED

#include "i2c_mem.h"
#include <string.h>

extern	I2C_DriverStruct_t	I2C_DriverStruct[MAX_I2C_DEVICES];
extern	uint8_t				last_i2c_used_handle,i2c_driver_request;

uint32_t i2c_extflash_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
	if ( I2C_DriverStruct[handle].read != NULL )
		return I2C_DriverStruct[handle].read(handle,address,data,data_len);
	return 1;
}

uint32_t i2c_extflash_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
	if ( I2C_DriverStruct[handle].write != NULL )
		return I2C_DriverStruct[handle].write(handle,address,data,data_len);
	return 1;
}

uint32_t i2c_extflash_erase_blocks(uint8_t handle, uint32_t start_block,uint32_t number_of_blocks)
{
	if ( I2C_DriverStruct[handle].erase_blocks != NULL )
		return I2C_DriverStruct[handle].erase_blocks(handle,start_block,number_of_blocks);
	return 1;
}

uint32_t i2c_extflash_erase_chip(uint8_t handle)
{
	if ( I2C_DriverStruct[handle].erase_chip != NULL )
		return I2C_DriverStruct[handle].erase_chip(handle);
	return 1;
}

uint32_t i2c_extflash_get_id(uint8_t handle,uint8_t *data)
{
	if ( I2C_DriverStruct[handle].get_id != NULL )
		return I2C_DriverStruct[handle].get_id(handle,data);
	return 1;
}

uint32_t i2c_extflash_get_status(uint8_t handle)
{
	if ( I2C_DriverStruct[handle].get_status != NULL )
		return I2C_DriverStruct[handle].get_status(handle);
	return 1;
}

uint32_t i2c_extflash_get_flags(uint8_t handle)
{
	if ( I2C_DriverStruct[handle].get_flags != NULL )
		return I2C_DriverStruct[handle].get_flags(handle);
	return 1;
}

uint32_t i2c_extflash_set_flags(uint8_t handle, uint32_t flags)
{
	if ( I2C_DriverStruct[handle].set_flags != NULL )
		return I2C_DriverStruct[handle].set_flags(handle,flags);
	return 1;
}
#endif
