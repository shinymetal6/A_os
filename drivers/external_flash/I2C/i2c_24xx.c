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
 * i2c_24xx.c
 *
 *  Created on: Nov 4, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
#include "../../../kernel/kernel_opt.h"

#include "i2c_24xx.h"
#include <string.h>

extern	ExtFlash_DriverStruct_t		ExtFlashDriverStruct[MAX_EXTMEM_DRIVERS];
extern	uint8_t						last_extflash_used_handle,extflash_driver_request;

ITCM_AREA_CODE uint32_t i2c_24xx_get_status(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE uint32_t i2c_24xx_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].extflash_driver_private_data;
	i2c_24xx_Drv->status = 0;
	if (( i2c_24xx_Drv->flags & USE_READ_DMA ) == USE_READ_DMA)
		return HAL_I2C_Mem_Read_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1);
	else
		return HAL_I2C_Mem_Read(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1, I2C_24XX_TIMEOUT);
}

ITCM_AREA_CODE uint32_t i2c_24xx_read_config(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].extflash_driver_private_data;
	return HAL_I2C_Mem_Read(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address | 0x10, address, i2c_24xx_Drv->device_address_size, data, data_len, I2C_24XX_TIMEOUT);
}

ITCM_AREA_CODE uint32_t i2c_24xx_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].extflash_driver_private_data;
	return HAL_I2C_Mem_Write(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1, I2C_24XX_TIMEOUT);
}

ITCM_AREA_CODE uint32_t i2c_24xx_init(uint8_t handle)
{
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].extflash_driver_private_data;
	i2c_24xx_Drv->status = I2C_24XX_STOPPED;
	if ( i2c_24xx_Drv->power_port != NULL )
	{
		if ( i2c_24xx_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(i2c_24xx_Drv->power_port, i2c_24xx_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(i2c_24xx_Drv->power_port, i2c_24xx_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t	i2c_24xx_register(i2c_24xx_Drv_TypeDef *extflash_driver_private_data,uint32_t driver_flags,uint32_t extflash_flags)
{
Pwm_Control_TypeDef	*extflash_driver_data;
	if ( ExtFlashDriverStruct[last_extflash_used_handle].process == 0 )
	{
		ExtFlashDriverStruct[last_extflash_used_handle].process = get_current_process();
		ExtFlashDriverStruct[last_extflash_used_handle].flags |= driver_flags;
		ExtFlashDriverStruct[last_extflash_used_handle].extflash_driver_private_data = (uint32_t *)extflash_driver_private_data;

		extflash_driver_data = (Pwm_Control_TypeDef *)ExtFlashDriverStruct[last_extflash_used_handle].extflash_driver_private_data;
		extflash_driver_data->flags |= extflash_flags;
		ExtFlashDriverStruct[last_extflash_used_handle].status = DRIVER_STATUS_REQUESTED;

		last_extflash_used_handle++;
		extflash_driver_request++;
		return last_extflash_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

/****** Interrupt **********/

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
uint32_t	i;
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	for(i=0;i<MAX_SENSORS;i++)
	{
		i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef *)ExtFlashDriverStruct[i].extflash_driver_private_data;
		if ( i2c_24xx_Drv->bus == hi2c )
		{
			i2c_24xx_Drv->status = READ_DMA_COMPLETE;
			return ;
		}
	}
}

