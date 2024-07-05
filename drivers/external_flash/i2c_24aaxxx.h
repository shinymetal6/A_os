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
 * i2c_24aaxxx.h
 *
 *  Created on: Jul 4, 2024
 *      Author: fil
 */

#ifndef DRIVERS_EXTERNAL_FLASH_I2C_24AAXXX_H_
#define DRIVERS_EXTERNAL_FLASH_I2C_24AAXXX_H_

#ifdef I2CFLASH_ENABLED

#define	HW_I2C_ERROR_HW_NOT_OWNED		1
#define	HW_I2C_ERROR_DEVICE_NOT_PRESENT	2
#define	HW_I2C_ERROR_HW_NOT_INITIALIZED	3
#define	HW_I2C_ERROR_SIZE_TOO_BIG		4

#define	I2C_24AAXXX_PAGE_SIZE	64

extern	void task_delay(uint32_t tick_count);

extern	uint8_t  i2c_24aaxxx_init(uint32_t i2c,uint16_t dev_addr);
extern	uint32_t i2c_24aaxxx_read (uint32_t i2c,uint16_t addr,uint8_t* data, uint16_t size);
extern	uint32_t i2c_24aaxxx_write(uint32_t i2c,uint16_t addr,uint8_t* data, uint16_t size);

#endif
#endif /* DRIVERS_EXTERNAL_FLASH_I2C_24AAXXX_H_ */
