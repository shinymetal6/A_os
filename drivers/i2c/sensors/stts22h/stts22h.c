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
 * stts22h.c
 *
 *  Created on: Dec 23, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "stts22h.h"

extern	I2C_DriverStruct_t	I2C_DriverStruct[MAX_I2C_DEVICES];
extern	uint8_t				last_i2c_used_handle,i2c_driver_request,i2c_busy_timeout;


ITCM_AREA_CODE	static uint8_t	stts22h_read_reg(I2C_Sensors_DriverStruct_t *stts22h_Drv,uint8_t address,uint8_t size)
{
uint8_t	ret = HAL_BUSY;

	stts22h_Drv->flags  &= ~I2C_STATUS_READ_COMPLETE;
	stts22h_Drv->i2c_timeout = I2C_BUSY_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Mem_Read(stts22h_Drv->bus, stts22h_Drv->device_address, address, stts22h_Drv->device_address_size, stts22h_Drv->data, size,STTS22H_I2C_TIMEOUT);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			stts22h_Drv->i2c_timeout--;
			if ( stts22h_Drv->i2c_timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE	static uint8_t	stts22h_write_reg(I2C_Sensors_DriverStruct_t *stts22h_Drv,uint8_t address,uint8_t data)
{
uint8_t	ret = HAL_BUSY;

	stts22h_Drv->flags  &= ~I2C_STATUS_READ_COMPLETE;
	stts22h_Drv->i2c_timeout = I2C_BUSY_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Mem_Write(stts22h_Drv->bus, stts22h_Drv->device_address, address, stts22h_Drv->device_address_size, &data, 1,STTS22H_I2C_TIMEOUT);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			stts22h_Drv->i2c_timeout--;
			if ( stts22h_Drv->i2c_timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t stts22h_start(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*stts22h_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( get_and_set_i2c_bus_lock(stts22h_Drv->bus,handle) == 0 )
	{
		stts22h_Drv->status = STTS22H_STARTED;
		if ( stts22h_write_reg(stts22h_Drv,STTS22H_CTRL,STTS22H_ONE_SHOT_REG_CTRL) )
			return 1;
		unset_i2c_bus_lock(stts22h_Drv->bus,handle);
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE static uint32_t stts22h_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t stts22h_init(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*stts22h_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( get_and_set_i2c_bus_lock(stts22h_Drv->bus,handle) == 0 )
	{
		stts22h_read_reg(stts22h_Drv,STTS22H_WHOAMI,1);
		stts22h_Drv->who_am_i = stts22h_Drv->data[0];
		unset_i2c_bus_lock(stts22h_Drv->bus,handle);
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE static uint32_t stts22h_power_on(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*stts22h_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;;
	if ( stts22h_Drv->power_port != NULL )
	{
		if ( stts22h_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(stts22h_Drv->power_port, stts22h_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(stts22h_Drv->power_port, stts22h_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t stts22h_power_off(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*stts22h_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;;
	if ( stts22h_Drv->power_port != NULL )
	{
		if ( stts22h_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(stts22h_Drv->power_port, stts22h_Drv->power_bit, GPIO_PIN_RESET);
		else
			  HAL_GPIO_WritePin(stts22h_Drv->power_port, stts22h_Drv->power_bit, GPIO_PIN_SET);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t stts22h_get_data(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*stts22h_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;;
	if ( get_and_set_i2c_bus_lock(stts22h_Drv->bus,handle) == 0 )
	{
		if ( stts22h_read_reg(stts22h_Drv,STTS22H_TEMP_L_OUT,STTS22H_T_LEN) )
			return 1;
		unset_i2c_bus_lock(stts22h_Drv->bus,handle);
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t stts22h_register(I2C_Sensors_DriverStruct_t *driver_private_data)
{
I2C_Sensors_DriverStruct_t	*stts22h_Drv;

	if ( I2C_DriverStruct[last_i2c_used_handle].process == 0 )
	{
		I2C_DriverStruct[last_i2c_used_handle].process = get_current_process();
		I2C_DriverStruct[last_i2c_used_handle].private_data = (uint32_t *)driver_private_data;
		I2C_DriverStruct[last_i2c_used_handle].handle = last_i2c_used_handle;

		stts22h_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[last_i2c_used_handle].private_data;
		if ( stts22h_Drv->bus == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( stts22h_Drv->data == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( stts22h_Drv->wakeup_id == 0)
			return DRIVER_REQUEST_FAILED;
		I2C_DriverStruct[last_i2c_used_handle].bus = stts22h_Drv->bus;

		stts22h_Drv->status = DRIVER_STATUS_IN_USE;
		stts22h_Drv->sensor_start = stts22h_start;
		stts22h_Drv->sensor_stop = stts22h_stop;
		stts22h_Drv->sensor_init = stts22h_init;
		stts22h_Drv->sensor_get_data = stts22h_get_data;
		stts22h_Drv->sensor_power_on = stts22h_power_on;
		stts22h_Drv->sensor_power_off = stts22h_power_off;
		stts22h_Drv->power_port = stts22h_Drv->power_port;
		stts22h_Drv->power_bit = stts22h_Drv->power_bit;
		last_i2c_used_handle++;
		i2c_driver_request++;
		return last_i2c_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;

	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED
