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
 * i2c_mem.h
 *
 *  Created on: Jan 8, 2025
 *      Author: fil
 */
#ifndef DRIVERS_I2C_MEMORIES_I2C_MEM_H_
#define DRIVERS_I2C_MEMORIES_I2C_MEM_H_

extern 	uint32_t	i2c_extflash_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
extern 	uint32_t	i2c_extflash_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
extern 	uint32_t	i2c_extflash_erase_blocks(uint8_t handle, uint32_t start_block,uint32_t number_of_blocks);
extern 	uint32_t	i2c_extflash_erase_chip(uint8_t handle);
extern 	uint32_t	i2c_extflash_get_id(uint8_t handle,uint8_t *data);
extern 	uint32_t	i2c_extflash_get_status(uint8_t handle);
extern 	uint32_t	i2c_extflash_get_flags(uint8_t handle);
extern	uint32_t 	i2c_extflash_set_flags(uint8_t handle, uint32_t flags);

#include "mem_24xx/i2c_24xx.h"

#endif /* DRIVERS_I2C_MEMORIES_I2C_MEM_H_ */
