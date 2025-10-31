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
 *  Created on: Oct 31, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "sht40.h"

ITCM_AREA_CODE uint32_t sht40_i2cread(I2C_Sht40_Drv_TypeDef *sht40_Drv)
{
uint8_t	ret = HAL_BUSY;

	sht40_Drv->flags  &= ~I2C_STATUS_READ_COMPLETE;
	sht40_Drv->timeout = SHT40_I2C_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Master_Receive_IT(sht40_Drv->bus,sht40_Drv->device_address, sht40_Drv->data, 6);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			sht40_Drv->timeout--;
			if ( sht40_Drv->timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_i2cwrite(I2C_Sht40_Drv_TypeDef *sht40_Drv)
{
uint8_t	ret = HAL_BUSY;
uint8_t		cmd = 0xfd;

	sht40_Drv->flags  &= ~I2C_STATUS_WRITE_COMPLETE;
	sht40_Drv->timeout = SHT40_I2C_TIMEOUT;
	while(ret == HAL_BUSY )
	{
		ret = HAL_I2C_Master_Transmit_IT(sht40_Drv->bus, sht40_Drv->device_address, &cmd,1);
		if ( ret == HAL_BUSY)
		{
			task_delay(1);
			sht40_Drv->timeout--;
			if ( sht40_Drv->timeout == 0 )
				return 1;
		}
	}
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_start(I2C_Sht40_Drv_TypeDef *sht40_Drv)
{
	sht40_Drv->status = SHT40_STARTED;
	if ( sht40_i2cwrite(sht40_Drv) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_get_data(I2C_Sht40_Drv_TypeDef *sht40_Drv)
{
	if ( sht40_i2cread(sht40_Drv) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_init(I2C_Sht40_Drv_TypeDef *sht40_Drv)
{
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

ITCM_AREA_CODE uint32_t sht40_power_on(I2C_Sht40_Drv_TypeDef *sht40_Drv)
{
	if ( sht40_Drv->power_port != NULL )
	{
		if ( sht40_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_power_off(I2C_Sht40_Drv_TypeDef *sht40_Drv)
{
	if ( sht40_Drv->power_port != NULL )
	{
		if ( sht40_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_RESET);
		else
			  HAL_GPIO_WritePin(sht40_Drv->power_port, sht40_Drv->power_bit, GPIO_PIN_SET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t sht40_register(I2C_Sht40_Drv_TypeDef *sht40_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;
	if ( sht40_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)sht40_Drv;
		sht40_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)sht40_Drv;
		sht40_Drv->next_drv = NULL;
	}
	sht40_Drv->process = get_current_process();
	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED


