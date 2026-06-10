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
 *  Created on: Oct 31, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "stts22h.h"

ITCM_AREA_CODE	uint8_t	stts22h_read_reg(I2C_Stts22H_DriverStruct_t *stts22h_Drv,uint8_t address,uint8_t size)
{
uint8_t	ret = HAL_BUSY;

	stts22h_Drv->flags  &= ~I2C_STATUS_READ_COMPLETE;
	stts22h_Drv->timeout = STTS22H_I2C_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Mem_Read(stts22h_Drv->bus, stts22h_Drv->device_address, address, stts22h_Drv->device_address_size, stts22h_Drv->data, size,STTS22H_I2C_TIMEOUT);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			stts22h_Drv->timeout--;
			if ( stts22h_Drv->timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE	uint8_t	stts22h_write_reg(I2C_Stts22H_DriverStruct_t *stts22h_Drv,uint8_t address,uint8_t data)
{
uint8_t	ret = HAL_BUSY;

	stts22h_Drv->flags  &= ~I2C_STATUS_READ_COMPLETE;
	stts22h_Drv->timeout = STTS22H_I2C_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Mem_Write(stts22h_Drv->bus, stts22h_Drv->device_address, address, stts22h_Drv->device_address_size, &data, 1,STTS22H_I2C_TIMEOUT);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			stts22h_Drv->timeout--;
			if ( stts22h_Drv->timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE uint32_t stts22h_start(I2C_Stts22H_DriverStruct_t *stts22h_Drv)
{
	stts22h_Drv->status = STTS22H_STARTED;
	if ( stts22h_write_reg(stts22h_Drv,STTS22H_CTRL,STTS22H_ONE_SHOT_REG_CTRL) )
		return 1;
	return 0;
}

ITCM_AREA_CODE  uint32_t stts22h_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE  uint32_t stts22h_init(I2C_Stts22H_DriverStruct_t *stts22h_Drv)
{
	stts22h_read_reg(stts22h_Drv,STTS22H_WHOAMI,1);
	stts22h_Drv->who_am_i = stts22h_Drv->data[0];
	return 0;
}

ITCM_AREA_CODE uint32_t stts22h_power_on(I2C_Stts22H_DriverStruct_t *stts22h_Drv)
{
	if ( stts22h_Drv->power_port != NULL )
	{
		if ( stts22h_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(stts22h_Drv->power_port, stts22h_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(stts22h_Drv->power_port, stts22h_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t stts22h_power_off(I2C_Stts22H_DriverStruct_t *stts22h_Drv)
{
	if ( stts22h_Drv->power_port != NULL )
	{
		if ( stts22h_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(stts22h_Drv->power_port, stts22h_Drv->power_bit, GPIO_PIN_RESET);
		else
			  HAL_GPIO_WritePin(stts22h_Drv->power_port, stts22h_Drv->power_bit, GPIO_PIN_SET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t stts22h_get_data(I2C_Stts22H_DriverStruct_t *stts22h_Drv)
{
	if ( stts22h_read_reg(stts22h_Drv,STTS22H_TEMP_L_OUT,STTS22H_T_LEN) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t stts22h_register(I2C_Stts22H_DriverStruct_t *stts22h_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;
	if ( stts22h_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)stts22h_Drv;
		stts22h_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)stts22h_Drv;
		stts22h_Drv->next_drv = NULL;
	}
	stts22h_Drv->process = get_current_process();
	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED


