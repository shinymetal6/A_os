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
 *  Created on: Mar 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
#include "../../../kernel/kernel_opt.h"

#include "lis2dw12.h"
#include <string.h>

extern	DriverStruct_t		*DriverStruct[MAX_DRIVERS];


static uint8_t	read_lis_reg(I2C_HandleTypeDef 	*bus,uint16_t device_address,uint8_t internal_address,uint8_t *pData)
{
	if ( HAL_I2C_Mem_Read(bus, device_address, internal_address, 1, pData, 1, STTS22H_I2C_TIMEOUT) != 0 )
		return 255;
	return pData[0];
}

static uint8_t	write_lis_reg(I2C_HandleTypeDef *bus,uint16_t device_address,uint8_t internal_address,uint8_t *pData)
{
	return 	HAL_I2C_Mem_Write(bus, device_address, internal_address, 1, pData, 1, STTS22H_I2C_TIMEOUT);

}

static uint32_t lis2dw12_start(uint8_t handle)
{
Lis2DW12_Drv_TypeDef	*lis2dw12_Drv;
	if ( DriverStruct[handle]->process == Asys.current_process)
	{
		lis2dw12_Drv = (Lis2DW12_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
		lis2dw12_Drv->status = LIS2DW12_STARTED;
		return HAL_I2C_Master_Transmit(lis2dw12_Drv->bus,lis2dw12_Drv->device_address, (uint8_t *)&lis2dw12_Drv->opmode, 1, SHT40_I2C_TIMEOUT);
	}
	else
		return LIS2DW12_DRIVER_NOT_OWNED;
}

static uint32_t lis2dw12_stop(uint8_t handle)
{
	return 0;
}

static uint32_t lis2dw12_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t lis2dw12_get_values(uint8_t handle,uint8_t *data,uint8_t datalen)
{
Lis2DW12_Drv_TypeDef	*lis2dw12_Drv;
uint32_t	ret_i2c_code;
	if ( DriverStruct[handle]->process == Asys.current_process)
	{
		lis2dw12_Drv = (Lis2DW12_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
		ret_i2c_code =  HAL_I2C_Master_Receive(lis2dw12_Drv->bus,lis2dw12_Drv->device_address, data, datalen, SHT40_I2C_TIMEOUT);
		if ( ret_i2c_code == 0 )
			return datalen;
		return ret_i2c_code;
	}
	else
		return LIS2DW12_DRIVER_NOT_OWNED;
}

static uint32_t lis2dw12_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t lis2dw12_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	DriverStruct_t	Lis2DW12_Def_Drv;

uint32_t lis2dw12_deinit(uint8_t handle)
{
	return driver_unregister(&Lis2DW12_Def_Drv);
}

static uint32_t lis2dw12_init(uint8_t handle)
{
Lis2DW12_Drv_TypeDef	*lis2dw12_Drv;
	lis2dw12_Drv = (Lis2DW12_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	lis2dw12_Drv->status = LIS2DW12_STARTED;
	if ( read_lis_reg(lis2dw12_Drv->bus,lis2dw12_Drv->device_address,LIS2DW12_WHO_AM_I,&lis2dw12_Drv->whoami) == LIS2DW12_ID)
	{
		if ( lis2dw12_Drv->power_port != NULL )
		{
			if ( lis2dw12_Drv->power_active_level == 1 )
				  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_SET);
			else
				  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_RESET);
		}
	}
	return 0;
}

DriverStruct_t	Lis2DW12_Def_Drv =
{
	.init = lis2dw12_init,
	.deinit = lis2dw12_deinit,
	.start = lis2dw12_start,
	.stop = lis2dw12_stop,
	.extended_action = lis2dw12_extended_actions,
	.get_status = lis2dw12_get_status,
	.get_values = lis2dw12_get_values,
	.set_values = lis2dw12_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "lis2dw12",
};

uint32_t lis2dw12_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&Lis2DW12_Def_Drv,sizeof(Lis2DW12_Def_Drv));
	return 0;
}

