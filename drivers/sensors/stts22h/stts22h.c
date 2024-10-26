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
 *  Created on: Oct 26, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#include "stts22h.h"
#include <string.h>

extern	DriverStruct_t		*DriverStruct[MAX_DRIVERS];

static uint8_t	read_stts22h_reg(I2C_HandleTypeDef 	*bus,uint16_t device_address,uint8_t internal_address,uint8_t *pData)
{
	HAL_I2C_Mem_Read(bus, device_address, internal_address, 1, pData, 1, STTS22H_I2C_TIMEOUT);
	return pData[0];
}

static uint8_t	write_stts22h_reg(I2C_HandleTypeDef *bus,uint16_t device_address,uint8_t internal_address,uint8_t *pData)
{
	return 	HAL_I2C_Mem_Write(bus, device_address, internal_address, 1, pData, 1, STTS22H_I2C_TIMEOUT);

}

static uint32_t stts22h_start(uint8_t handle)
{
Stts22h_Drv_TypeDef	*Stts22h_Drv;
uint8_t	start_cmd = STTS22H_ONE_SHOT_REG_CTRL;
	Stts22h_Drv = (Stts22h_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	return write_stts22h_reg(Stts22h_Drv->bus,Stts22h_Drv->device_address,STTS22H_CTRL,&start_cmd);
}

static uint32_t stts22h_stop(uint8_t handle)
{
	return 0;
}

static uint32_t stts22h_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t stts22h_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t stts22h_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t stts22h_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	DriverStruct_t	Stts22h_Def_Drv;

uint32_t stts22h_deinit(uint8_t handle)
{
	return driver_unregister(&Stts22h_Def_Drv);
}

static uint32_t stts22h_init(uint8_t handle)
{
Stts22h_Drv_TypeDef	*Stts22h_Drv;
	Stts22h_Drv = (Stts22h_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	Stts22h_Drv->status = SHT40_STOPPED;
	if ( Stts22h_Drv->power_port != NULL )
	{
		if ( Stts22h_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(SENSORS_POWER_GPIO_Port, SENSORS_POWER_Pin, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(SENSORS_POWER_GPIO_Port, SENSORS_POWER_Pin, GPIO_PIN_RESET);

	}
	read_stts22h_reg(Stts22h_Drv->bus,Stts22h_Drv->device_address,STTS22H_WHOAMI,&Stts22h_Drv->whoami);

	return 0;
}

DriverStruct_t	Stts22h_Def_Drv =
{
	.init = stts22h_init,
	.deinit = stts22h_deinit,
	.start = stts22h_start,
	.stop = stts22h_stop,
	.extended_action = stts22h_extended_actions,
	.get_status = stts22h_get_status,
	.get_values = stts22h_get_values,
	.set_values = stts22h_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "stts22h",
};

uint32_t stts22h_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&Stts22h_Def_Drv,sizeof(Stts22h_Def_Drv));
	return 0;
}



