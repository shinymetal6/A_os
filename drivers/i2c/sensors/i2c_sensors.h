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
 * i2c_sensors.h
 *
 *  Created on: Dec 22, 2024
 *      Author: fil
 */
#ifndef DRIVERS_I2C_SENSORS_I2C_SENSORS_H_
#define DRIVERS_I2C_SENSORS_I2C_SENSORS_H_
#ifdef A_OS_I2C_ENABLED

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint32_t			sensor_id;
	uint32_t			who_am_i;
	uint8_t				*data;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint16_t 			device_address_size;
	GPIO_TypeDef	 	*power_port;
	uint16_t			power_bit;
	uint16_t			power_active_level;
	uint32_t 			wakeup_id;
	uint32_t			(*sensor_init)(uint8_t);
	uint32_t			(*sensor_start)(uint8_t);
	uint32_t			(*sensor_stop)(uint8_t);
	uint32_t			(*sensor_get_data)(uint8_t);
	uint32_t			(*sensor_power_on)(uint8_t);
	uint32_t			(*sensor_power_off)(uint8_t);
}I2C_Sensors_DriverStruct_t;

#endif // #ifdef A_OS_I2C_ENABLED

#include "sht40/sht40.h"
#include "lps22df/lps22df.h"

extern	uint32_t sensor_init(uint8_t handle);
extern	uint32_t sensor_start(uint8_t handle);
extern	uint32_t sensor_get_data(uint8_t handle);
extern	uint32_t sensor_power_on(uint8_t handle);
extern	uint32_t sensor_power_off(uint8_t handle);



#endif /* DRIVERS_I2C_SENSORS_I2C_SENSORS_H_ */
