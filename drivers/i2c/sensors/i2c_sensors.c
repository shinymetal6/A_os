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
 * i2c_sensors.c
 *
 *  Created on: Dec 22, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_I2C_ENABLED

#include "i2c_sensors.h"
#include <string.h>
extern	I2C_DriverStruct_t	I2C_DriverStruct[MAX_I2C_DEVICES];
extern	uint8_t				last_i2c_used_handle,i2c_driver_request;

ITCM_AREA_CODE uint32_t	sensor_init(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*i2c_sensor_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[handle].private_data;
	if ( i2c_sensor_Drv->sensor_init != NULL)
		i2c_sensor_Drv->sensor_init(handle);
	return 0;
}

ITCM_AREA_CODE uint32_t	sensor_start(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*i2c_sensor_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[handle].private_data;
	if ( i2c_sensor_Drv->sensor_start != NULL)
		i2c_sensor_Drv->sensor_start(handle);
	return 0;
}

ITCM_AREA_CODE uint32_t	sensor_stop(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*i2c_sensor_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[handle].private_data;
	if ( i2c_sensor_Drv->sensor_stop != NULL)
		i2c_sensor_Drv->sensor_stop(handle);
	return 0;
}

ITCM_AREA_CODE uint32_t	sensor_get_data(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*i2c_sensor_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[handle].private_data;
	if ( i2c_sensor_Drv->sensor_get_data != NULL)
		i2c_sensor_Drv->sensor_get_data(handle);
	return 0;
}

ITCM_AREA_CODE uint32_t	sensor_power_on(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*i2c_sensor_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[handle].private_data;
	if ( i2c_sensor_Drv->sensor_power_on != NULL)
		i2c_sensor_Drv->sensor_power_on(handle);
	return 0;
}

ITCM_AREA_CODE uint32_t	sensor_power_off(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*i2c_sensor_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[handle].private_data;
	if ( i2c_sensor_Drv->sensor_power_off != NULL)
		i2c_sensor_Drv->sensor_power_off(handle);
	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED


