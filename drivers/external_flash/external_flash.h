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
 * external_flash.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_EXTERNAL_FLASH_EXTERNAL_FLASH_H_
#define DRIVERS_EXTERNAL_FLASH_EXTERNAL_FLASH_H_

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	*driver_private_data;
	uint32_t	(*read)  (uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
	uint32_t	(*write) (uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
	uint32_t	(*erase_blocks) (uint8_t handle, uint32_t start_block, uint32_t number_of_blocks);
	uint32_t	(*erase_chip) (uint8_t handle);
	uint32_t	(*get_status) (uint8_t handle);
	uint32_t	(*get_flags) (uint8_t handle);
	uint32_t	(*set_flags) (uint8_t handle, uint32_t flags);
}ExtFlash_DriverStruct_t;

#include "I2C/i2c_24xx.h"
#include "QSPI/w25qxx.h"

extern	uint32_t extflash_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
extern	uint32_t extflash_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len);
extern	uint32_t extflash_erase_blocks(uint8_t handle, uint32_t start_block,uint32_t number_of_blocks);
extern	uint32_t extflash_erase_chip(uint8_t handle);
extern	uint32_t extflash_get_status(uint8_t handle);
extern	uint32_t extflash_get_flags(uint8_t handle);
extern	uint32_t extflash_set_flags(uint8_t handle, uint32_t flags);

#endif /* DRIVERS_EXTERNAL_FLASH_EXTERNAL_FLASH_H_ */
