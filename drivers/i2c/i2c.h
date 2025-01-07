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
 * i2c.h
 *
 *  Created on: Dec 21, 2024
 *      Author: fil
 */
#ifndef DRIVERS_I2C_I2C_H_
#define DRIVERS_I2C_I2C_H_

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	I2C_HandleTypeDef 	*bus;
	uint32_t			*private_data;
}I2C_DriverStruct_t;

/* status */
#define	I2C_STATUS_DEVICE_ERROR				0x80
#define	I2C_STATUS_ERROR					0x40
#define	I2C_STATUS_DEVICE_BUSY				0x20
#define	I2C_STATUS_DRIVER_NOT_OWNED			0x10
#define	I2C_STATUS_BUSY						0x08
#define	I2C_STATUS_READY					0x04
#define	I2C_STATUS_READ_COMPLETE			0x02
#define	I2C_STATUS_WRITE_COMPLETE			0x01

/* flags */
#define	I2C_FLAGS_WAIT_ON_WRITE_COMPLETE	0x80
#define	I2C_FLAGS_WAIT_ON_READ_COMPLETE		0x40
#define	I2C_FLAGS_USES_WRITE_DMA			0x08
#define	I2C_FLAGS_USES_READ_DMA				0x04
#define	I2C_FLAGS_WAKEUP_ON_READ			0x02
#define	I2C_FLAGS_WAKEUP_ON_WRITE			0x01

#define	I2C_BUSY_TIMEOUT					100
#include "sensors/i2c_sensors.h"

extern	uint8_t get_and_set_i2c_bus_lock(I2C_HandleTypeDef *hi2c,uint8_t handle);
extern	uint8_t unset_i2c_bus_lock(I2C_HandleTypeDef *hi2c,uint8_t handle);
extern	uint8_t get_and_set_i2cmem_bus_lock(I2C_HandleTypeDef *hi2c,uint8_t handle);
extern	uint8_t unset_i2cmem_bus_lock(I2C_HandleTypeDef *hi2c,uint8_t handle);

#endif /* DRIVERS_I2C_I2C_H_ */
