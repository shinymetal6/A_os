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
 * sht40.c
 *
 *  Created on: Mar 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"

#ifdef A_OS_I2C_ENABLED
#include "../../../kernel/A_exported_functions.h"
#include "sht40.h"

extern	Sensors_DriverStruct_t	Sensors_DriverStruct[MAX_I2C_DEVICES];
extern	uint8_t					last_sensor_used_handle,sensor_driver_request;;

ITCM_AREA_CODE static uint32_t sht40_start(uint8_t handle)
{
Sht40_Drv_TypeDef	*sht40_Drv;
	if ( Sensors_DriverStruct[handle].process == Asys.current_process)
	{
		sht40_Drv = (Sht40_Drv_TypeDef	*)Sensors_DriverStruct[handle].private_data;
		sht40_Drv->status = SHT40_STARTED;
		return HAL_I2C_Master_Transmit(sht40_Drv->bus,sht40_Drv->device_address, (uint8_t *)&sht40_Drv->precision, 1, SHT40_I2C_TIMEOUT);
	}
	else
		return SHT40_DRIVER_NOT_OWNED;
}

ITCM_AREA_CODE static uint32_t sht40_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t sht40_get_values(uint8_t handle,uint8_t *data,uint16_t datalen)
{
Sht40_Drv_TypeDef	*sht40_Drv;
uint32_t	ret_i2c_code;
	if ( Sensors_DriverStruct[handle].process == Asys.current_process)
	{
		sht40_Drv = (Sht40_Drv_TypeDef	*)Sensors_DriverStruct[handle].private_data;
		ret_i2c_code =  HAL_I2C_Master_Receive(sht40_Drv->bus,sht40_Drv->device_address, data, datalen, SHT40_I2C_TIMEOUT);
		if ( ret_i2c_code == 0 )
			return datalen;
		return ret_i2c_code;
	}
	else
		return SHT40_DRIVER_NOT_OWNED;
}

ITCM_AREA_CODE static uint32_t sht40_init(uint8_t handle)
{
Sht40_Drv_TypeDef	*sht40_Drv;
	sht40_Drv = (Sht40_Drv_TypeDef	*)Sensors_DriverStruct[handle].private_data;
	sht40_Drv->status = SHT40_STOPPED;
	if ( sht40_Drv->power_port != NULL )
	{
		if ( sht40_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_register(Sht40_Drv_TypeDef *driver_private_data,uint32_t driver_flags)
{
Sht40_Drv_TypeDef	*sht40_Drv;
	if ( Sensors_DriverStruct[last_sensor_used_handle].process == 0 )
	{
		Sensors_DriverStruct[last_sensor_used_handle].process = get_current_process();
		Sensors_DriverStruct[last_sensor_used_handle].flags |= driver_flags;
		Sensors_DriverStruct[last_sensor_used_handle].private_data = (uint32_t *)driver_private_data;

		sht40_Drv = (Sht40_Drv_TypeDef *)Sensors_DriverStruct[last_sensor_used_handle].private_data;
		if ( sht40_Drv->bus == NULL)
			return DRIVER_REQUEST_FAILED;
		Sensors_DriverStruct[last_sensor_used_handle].status = DRIVER_STATUS_IN_USE;
		Sensors_DriverStruct[last_sensor_used_handle].sensor_start = sht40_start;
		Sensors_DriverStruct[last_sensor_used_handle].sensor_stop = sht40_stop;
		Sensors_DriverStruct[last_sensor_used_handle].sensor_init = sht40_init;
		last_sensor_used_handle++;
		sensor_driver_request++;
		return last_sensor_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;

	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED
