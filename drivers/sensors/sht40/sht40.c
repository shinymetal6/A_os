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
 * .c
 *
 *  Created on: Oct 26, 2024
 *      Author: fil
 */


#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#include "sht40.h"
#include <string.h>

extern	DriverStruct_t		*DriverStruct[MAX_DRIVERS];

static uint32_t sht40_start(uint8_t handle)
{
Sht40_Drv_TypeDef	*sht40_Drv;
	if ( DriverStruct[handle]->process == Asys.current_process)
	{
		sht40_Drv = (Sht40_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
		sht40_Drv->status = SHT40_STARTED;
		return HAL_I2C_Master_Transmit(sht40_Drv->bus,sht40_Drv->device_address, (uint8_t *)&sht40_Drv->precision, 1, SHT40_I2C_TIMEOUT);
	}
	else
		return SHT40_DRIVER_NOT_OWNED;
}

static uint32_t sht40_stop(uint8_t handle)
{
	return 0;
}

static uint32_t sht40_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t sht40_get_values(uint8_t handle,uint8_t *data,uint8_t datalen)
{
Sht40_Drv_TypeDef	*sht40_Drv;
uint32_t	ret_i2c_code;
	if ( DriverStruct[handle]->process == Asys.current_process)
	{
		sht40_Drv = (Sht40_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
		ret_i2c_code =  HAL_I2C_Master_Receive(sht40_Drv->bus,sht40_Drv->device_address, data, datalen, SHT40_I2C_TIMEOUT);
		if ( ret_i2c_code == 0 )
			return datalen;
		return ret_i2c_code;
	}
	else
		return SHT40_DRIVER_NOT_OWNED;
}

static uint32_t sht40_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t sht40_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	DriverStruct_t	Sht40_Def_Drv;

uint32_t sht40_deinit(uint8_t handle)
{
	return driver_unregister(&Sht40_Def_Drv);
}

static uint32_t sht40_init(uint8_t handle)
{
Sht40_Drv_TypeDef	*sht40_Drv;
	sht40_Drv = (Sht40_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
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

DriverStruct_t	Sht40_Def_Drv =
{
	.init = sht40_init,
	.deinit = sht40_deinit,
	.start = sht40_start,
	.stop = sht40_stop,
	.extended_action = sht40_extended_actions,
	.get_status = sht40_get_status,
	.get_values = sht40_get_values,
	.set_values = sht40_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "sht40",
};

uint32_t sht40_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&Sht40_Def_Drv,sizeof(Sht40_Def_Drv));
	return 0;
}

