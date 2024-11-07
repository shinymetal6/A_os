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
 * i2c_24xx.h
 *
 *  Created on: Nov 4, 2024
 *      Author: fil
 */
#ifndef DRIVERS_EXTERNAL_FLASH_I2C_24XX_H_
#define DRIVERS_EXTERNAL_FLASH_I2C_24XX_H_

#include "../extmem_driver_manager.h"

#define	I2C_24XX_STOPPED	0
#define	I2C_24XX_TIMEOUT	1000
#define	I2C_24XX_ADDRESS	0xa0
#define	I2C_24XX_PAGESIZE	256

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				*data;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint8_t 			device_address_size;
	GPIO_TypeDef	 	*power_port;
	uint16_t			power_bit;
	uint16_t			power_active_level;
}i2c_24xx_Drv_TypeDef;

extern	uint32_t i2c_24xx_allocate_driver(MemDriverStruct_t *new_struct);

#endif /* DRIVERS_EXTERNAL_FLASH_I2C_24XX_H_ */
