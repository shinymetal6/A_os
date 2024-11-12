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
	uint32_t	*extflash_driver_private_data;
}ExtFlash_DriverStruct_t;

#include "I2C/i2c_24xx.h"

#endif /* DRIVERS_EXTERNAL_FLASH_EXTERNAL_FLASH_H_ */
