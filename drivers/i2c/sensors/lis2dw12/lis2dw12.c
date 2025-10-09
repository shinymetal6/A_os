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
 * lis2dw12.c
 *
 *  Created on: Jan 8, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "lis2dw12.h"

extern	I2C_DriverStruct_t	I2C_DriverStruct[MAX_I2C_DEVICES];
extern	uint8_t				last_i2c_used_handle,i2c_driver_request;

ITCM_AREA_CODE static uint8_t	write_lis_reg(I2C_Sensors_DriverStruct_t *lis2dw12_Drv,uint8_t address,uint8_t data)
{
	return HAL_I2C_Mem_Write(lis2dw12_Drv->bus, lis2dw12_Drv->device_address, address, 1, &data, 1,lis2dw12_Drv->i2c_timeout);
}

ITCM_AREA_CODE static uint8_t	read_lis_reg(I2C_Sensors_DriverStruct_t *lis2dw12_Drv,uint8_t address,uint8_t *data)
{
	return HAL_I2C_Mem_Read(lis2dw12_Drv->bus, lis2dw12_Drv->device_address, address, 1, data, 1,lis2dw12_Drv->i2c_timeout);
}

ITCM_AREA_CODE static uint32_t lis2dw12_start(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lis2dw12_Drv;
	lis2dw12_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	lis2dw12_Drv->status = LIS2DW12_STARTED;
	if ( get_and_set_i2c_bus_lock(lis2dw12_Drv->bus,handle) == 0 )
	{
		read_lis_reg( lis2dw12_Drv,LIS2DW12_WHO_AM_I,(uint8_t *)&lis2dw12_Drv->who_am_i);
		write_lis_reg( lis2dw12_Drv, LIS2DW12_CTRL1 , LIS2DW12_LP_MODE_1 | LIS2DW12_MODE_HIGH_PERF | LIS2DW12_ODR_100Hz );
		write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL6, LIS2DW12_BW_FILT_ODR20 | LIS2DW12_FS_16G);
		write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL2 , LIS2DW12_BOOT | LIS2DW12_BDU | LIS2DW12_IF_ADD_INC);
		write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL3 , 0x0);	// pulsed irq

		if (( lis2dw12_Drv->device_flags & IRQ_MODE_ON_WAKEUP ) == IRQ_MODE_ON_WAKEUP )
			write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL4_INT1_PAD_CTRL , LIS2DW12_INT1_WU);
		else
			write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL4_INT1_PAD_CTRL , LIS2DW12_INT1_FTH);

		write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL5_INT2_PAD_CTRL , 0);
		write_lis_reg( lis2dw12_Drv,LIS2DW12_WAKE_UP_THS ,  LIS2DW12_SINGLE_DOUBLE_TAP | 16);
		write_lis_reg( lis2dw12_Drv,LIS2DW12_WAKE_UP_DUR ,  0x10);
		write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL_REG7, LIS2DW12_DRDY_PULSED | LIS2DW12_INTERRUPTS_ENABLE | LIS2DW12_INT2_ON_INT1);

		write_lis_reg( lis2dw12_Drv,LIS2DW12_FIFO_CTRL, LISDW_FIFOMODE_BYPASS << 5 | (lis2dw12_Drv->device_config-1) );
		write_lis_reg( lis2dw12_Drv,LIS2DW12_FIFO_CTRL, LISDW_FIFOMODE_CONTINUOUS << 5 | (lis2dw12_Drv->device_config-1) );

		read_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL4_INT1_PAD_CTRL,&lis2dw12_Drv->status); // dummy read
		read_lis_reg( lis2dw12_Drv,LIS2DW12_STATUS,&lis2dw12_Drv->status);
		unset_i2c_bus_lock(lis2dw12_Drv->bus,handle);
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE static uint32_t lis2dw12_stop(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lis2dw12_Drv;
uint8_t temp;
	lis2dw12_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	lis2dw12_Drv->status = LIS2DW12_STOPPED;
	read_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL4_INT1_PAD_CTRL,&temp);
	write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL2 , temp | 0x40);
	read_lis_reg( lis2dw12_Drv,LIS2DW12_STATUS,&lis2dw12_Drv->status);
	return 0;
}

ITCM_AREA_CODE static uint32_t lis2dw12_get_Pdata(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lis2dw12_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	return HAL_I2C_Mem_Read(lis2dw12_Drv->bus, lis2dw12_Drv->device_address, LIS2DW12_OUT_X_L, 1, lis2dw12_Drv->data, MAG_LEN,lis2dw12_Drv->i2c_timeout);
}

ITCM_AREA_CODE static uint32_t lis2dw12_get_Tdata(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lis2dw12_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	return HAL_I2C_Mem_Read(lis2dw12_Drv->bus, lis2dw12_Drv->device_address, LIS2DW12_OUT_T, 1, lis2dw12_Drv->additional_data, 1,lis2dw12_Drv->i2c_timeout);
}

ITCM_AREA_CODE static uint32_t lis2dw12_power_on(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lis2dw12_Drv;
	lis2dw12_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( lis2dw12_Drv->power_port != NULL )
	{
		if ( lis2dw12_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t lis2dw12_power_off(uint8_t handle)
{
I2C_Sensors_DriverStruct_t	*lis2dw12_Drv;
	lis2dw12_Drv = (I2C_Sensors_DriverStruct_t	*)I2C_DriverStruct[handle].private_data;
	if ( lis2dw12_Drv->power_port != NULL )
	{
		if ( lis2dw12_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_RESET);
		else
			  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_SET);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t lis2dw12_init(uint8_t handle)
{
	return lis2dw12_power_on(handle);
}

ITCM_AREA_CODE uint32_t lis2dw12_register(I2C_Sensors_DriverStruct_t *driver_private_data)
{
I2C_Sensors_DriverStruct_t	*lis2dw12_Drv;

	if ( I2C_DriverStruct[last_i2c_used_handle].process == 0 )
	{
		I2C_DriverStruct[last_i2c_used_handle].process = get_current_process();
		I2C_DriverStruct[last_i2c_used_handle].private_data = (uint32_t *)driver_private_data;
		I2C_DriverStruct[last_i2c_used_handle].handle = last_i2c_used_handle;
		lis2dw12_Drv = (I2C_Sensors_DriverStruct_t *)I2C_DriverStruct[last_i2c_used_handle].private_data;
		if ( lis2dw12_Drv->bus == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( lis2dw12_Drv->data == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( lis2dw12_Drv->additional_data == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( lis2dw12_Drv->wakeup_id == 0)
			return DRIVER_REQUEST_FAILED;
		I2C_DriverStruct[last_i2c_used_handle].bus = lis2dw12_Drv->bus;

		if ( lis2dw12_Drv->i2c_timeout == 0 )
			lis2dw12_Drv->i2c_timeout = LIS2DW12_I2C_TIMEOUT;
		if ( lis2dw12_Drv->device_config == 0 )
			lis2dw12_Drv->device_config = LIS2DW12_NUM_FIFO_LOCATIONS;
		lis2dw12_Drv->status = DRIVER_STATUS_IN_USE;
		lis2dw12_Drv->sensor_start = lis2dw12_start;
		lis2dw12_Drv->sensor_stop = lis2dw12_stop;
		lis2dw12_Drv->sensor_init = lis2dw12_init;
		lis2dw12_Drv->sensor_get_data = lis2dw12_get_Pdata;
		lis2dw12_Drv->sensor_get_additional_data = lis2dw12_get_Tdata;

		lis2dw12_Drv->sensor_power_on = lis2dw12_power_on;
		lis2dw12_Drv->sensor_power_off = lis2dw12_power_off;
		lis2dw12_Drv->power_port = lis2dw12_Drv->power_port;
		lis2dw12_Drv->power_bit = lis2dw12_Drv->power_bit;
		last_i2c_used_handle++;
		i2c_driver_request++;
		return last_i2c_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;

	return 0;
}

#endif // #ifdef A_OS_I2C_ENABLED
