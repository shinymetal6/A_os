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
#include "../../../../kernel/system_default.h"

#ifdef A_OS_I2C_ENABLED
#include "../../../../kernel/A_exported_functions.h"
#include "sht40.h"

extern	I2C_DriverStruct_t	I2C_DriverStruct[MAX_I2C_DEVICES];
extern	uint8_t				last_i2c_used_handle,i2c_driver_request,i2c_busy_timeout;

ITCM_AREA_CODE static uint32_t sht40_i2cread(I2C_Sensors_DriverStruct_t *sht40_Drv)
{
uint8_t	ret = HAL_BUSY;

	sht40_Drv->flags  &= ~I2C_STATUS_READ_COMPLETE;
	sht40_Drv->i2c_timeout = I2C_BUSY_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Master_Receive_IT(sht40_Drv->bus,sht40_Drv->device_address, sht40_Drv->data, 6);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			sht40_Drv->i2c_timeout--;
			if ( sht40_Drv->i2c_timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t sht40_i2cwrite(I2C_Sensors_DriverStruct_t *sht40_Drv)
{
uint8_t	ret = HAL_BUSY;
uint8_t		cmd = 0xfd;

	sht40_Drv->flags  &= ~I2C_STATUS_WRITE_COMPLETE;
	sht40_Drv->i2c_timeout = I2C_BUSY_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Master_Transmit_IT(sht40_Drv->bus, sht40_Drv->device_address, &cmd,1);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			sht40_Drv->i2c_timeout--;
			if ( sht40_Drv->i2c_timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t sht40_start(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*sht40_Drv;
	sht40_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	sht40_Drv->status = SHT40_STARTED;
	return sht40_i2cwrite(sht40_Drv);
}

ITCM_AREA_CODE static uint32_t sht40_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t sht40_get_data(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*sht40_Drv;
	sht40_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	return sht40_i2cread(sht40_Drv);
}

ITCM_AREA_CODE static uint32_t sht40_init(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*sht40_Drv;
	sht40_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( sht40_Drv->power_port != NULL )
	{
		if ( sht40_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_RESET);
	}
	/* no whoami reg provided, so generate a fake one */
	sht40_Drv->who_am_i = ('S'<<24) | ('H'<<16) | ('T'<<8) | 0x20;
	return 0;
}

ITCM_AREA_CODE static uint32_t sht40_power_on(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*sht40_Drv;
	sht40_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( sht40_Drv->power_port != NULL )
	{
		if ( sht40_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t sht40_power_off(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*sht40_Drv;
	sht40_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( sht40_Drv->power_port != NULL )
	{
		if ( sht40_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_RESET);
		else
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_SET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_register(I2C_Sensors_DriverStruct_t *driver_private_data)
{
I2C_Sensors_DriverStruct_t	*sht40_Drv;

	if ( I2C_DriverStruct[last_i2c_used_handle].process == 0 )
	{
		I2C_DriverStruct[last_i2c_used_handle].process = get_current_process();
		I2C_DriverStruct[last_i2c_used_handle].private_data = (uint32_t *)driver_private_data;

		sht40_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[last_i2c_used_handle].private_data;
		if ( sht40_Drv->bus == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( sht40_Drv->data == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( sht40_Drv->wakeup_id == 0)
			return DRIVER_REQUEST_FAILED;
		sht40_Drv->status = DRIVER_STATUS_IN_USE;
		sht40_Drv->sensor_start = sht40_start;
		sht40_Drv->sensor_stop = sht40_stop;
		sht40_Drv->sensor_init = sht40_init;
		sht40_Drv->sensor_get_data = sht40_get_data;
		sht40_Drv->sensor_power_on = sht40_power_on;
		sht40_Drv->sensor_power_off = sht40_power_off;
		sht40_Drv->power_port = sht40_Drv->power_port;
		sht40_Drv->power_bit = sht40_Drv->power_bit;
		last_i2c_used_handle++;
		i2c_driver_request++;
		return last_i2c_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;

	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED
