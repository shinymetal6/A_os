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
 * lps22df.c
 *
 *  Created on: Dec 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/system_default.h"

#ifdef A_OS_I2C_ENABLED
#include "../../../../kernel/A_exported_functions.h"
#include "lps22df.h"

extern	I2C_DriverStruct_t	I2C_DriverStruct[MAX_I2C_DEVICES];
extern	uint8_t				last_i2c_used_handle,i2c_driver_request;

ITCM_AREA_CODE	static uint8_t	read_lps22df_reg(I2C_Sensors_DriverStruct_t *lps22df_Drv,uint8_t address)
{
uint8_t	data;
	if ( HAL_I2C_Mem_Read(lps22df_Drv->bus, lps22df_Drv->device_address, address, lps22df_Drv->device_address_size, &data, 1,LPS22DF_I2C_TIMEOUT) == 0)
		return data;
	return 0;
}

ITCM_AREA_CODE	static uint8_t	write_lps22df_reg(I2C_Sensors_DriverStruct_t *lps22df_Drv,uint8_t address,uint8_t data)
{
	return HAL_I2C_Mem_Write(lps22df_Drv->bus, lps22df_Drv->device_address, address, lps22df_Drv->device_address_size, &data, 1,LPS22DF_I2C_TIMEOUT);
}

ITCM_AREA_CODE static uint32_t lps22df_start(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lps22df_Drv;
	lps22df_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	lps22df_Drv->status = LPS22DF_STARTED;
	if ( get_and_set_i2c_bus_lock(lps22df_Drv->bus,handle) == 0 )
	{
		if ( write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG2,LPS22DF_ONE_SHOT_REG2) )
			return 1;
		unset_i2c_bus_lock(lps22df_Drv->bus,handle);
	}
	else
		return 1;

	return 0;
}

ITCM_AREA_CODE static uint32_t lps22df_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t lps22df_get_data(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lps22df_Drv;
	lps22df_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( get_and_set_i2c_bus_lock(lps22df_Drv->bus,handle) == 0 )
	{
		if ( HAL_I2C_Master_Receive(lps22df_Drv->bus,lps22df_Drv->device_address, lps22df_Drv->data,LPS22DF_P_LEN, LPS22DF_I2C_TIMEOUT) )
			return 1;
		unset_i2c_bus_lock(lps22df_Drv->bus,handle);
	}
	else
		return 1;
	return 0;
}

ITCM_AREA_CODE static uint32_t lps22df_init(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lps22df_Drv;
	lps22df_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( get_and_set_i2c_bus_lock(lps22df_Drv->bus,handle) == 0 )
	{
		if ( lps22df_Drv->power_port != NULL )
		{
			if ( lps22df_Drv->power_active_level == 1 )
				  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_SET);
			else
				  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_RESET);
		}
		lps22df_Drv->who_am_i = read_lps22df_reg(lps22df_Drv,LPS22DF_WHO_AM_I);

		write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG2,LPS22DF_RESET_REG2);
		write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG2,LPS22DF_BOOT_REG2);
		while((read_lps22df_reg(lps22df_Drv,LPS22DF_INT_SOURCE) & LPS22DF_BOOT_ON_REG_INT_SOURCE) == LPS22DF_BOOT_ON_REG_INT_SOURCE)
			task_delay(1);
		write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG2,LPS22DF_ONE_SHOT_REG2 | LPS22DF_LPF_REG2);
		write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG1,(LPS22DF_ONE_SHOT | LPS22DF_16_AVG));
		write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG4,LPS22DF_IE_REG4);
		unset_i2c_bus_lock(lps22df_Drv->bus,handle);
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE static uint32_t lps22df_power_on(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lps22df_Drv;
	lps22df_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( lps22df_Drv->power_port != NULL )
	{
		if ( lps22df_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t lps22df_power_off(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lps22df_Drv;
	lps22df_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( lps22df_Drv->power_port != NULL )
	{
		if ( lps22df_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_RESET);
		else
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_SET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t lps22df_register(I2C_Sensors_DriverStruct_t *driver_private_data)
{
I2C_Sensors_DriverStruct_t	*lps22df_Drv;

	if ( I2C_DriverStruct[last_i2c_used_handle].process == 0 )
	{
		I2C_DriverStruct[last_i2c_used_handle].process = get_current_process();
		I2C_DriverStruct[last_i2c_used_handle].private_data = (uint32_t *)driver_private_data;
		I2C_DriverStruct[last_i2c_used_handle].handle = last_i2c_used_handle;
		lps22df_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[last_i2c_used_handle].private_data;
		if ( lps22df_Drv->bus == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( lps22df_Drv->data == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( lps22df_Drv->wakeup_id == 0)
			return DRIVER_REQUEST_FAILED;
		I2C_DriverStruct[last_i2c_used_handle].bus = lps22df_Drv->bus;

		lps22df_Drv->status = DRIVER_STATUS_IN_USE;
		lps22df_Drv->sensor_start = lps22df_start;
		lps22df_Drv->sensor_stop = lps22df_stop;
		lps22df_Drv->sensor_init = lps22df_init;
		lps22df_Drv->sensor_get_data = lps22df_get_data;
		lps22df_Drv->sensor_power_on = lps22df_power_on;
		lps22df_Drv->sensor_power_off = lps22df_power_off;
		lps22df_Drv->power_port = lps22df_Drv->power_port;
		lps22df_Drv->power_bit = lps22df_Drv->power_bit;
		last_i2c_used_handle++;
		i2c_driver_request++;
		return last_i2c_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;

	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED
