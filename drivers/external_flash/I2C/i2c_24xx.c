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
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
	return i2c_24xx_Drv->status;
}

ITCM_AREA_CODE uint32_t i2c_24xx_get_flags(uint8_t handle)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
	return i2c_24xx_Drv->flags;
}

ITCM_AREA_CODE uint32_t i2c_24xx_set_flags(uint8_t handle, uint32_t flags)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
	i2c_24xx_Drv->flags = flags;
	return  0;
}

ITCM_AREA_CODE uint32_t i2c_24xx_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
uint32_t ret_val;

	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
	i2c_24xx_Drv->status = 0;
	while(data_len > I2C_24XX_PAGESIZE)
	{
		if (( i2c_24xx_Drv->flags & I2C_USES_READ_DMA ) == I2C_USES_READ_DMA)
		{
			i2c_24xx_Drv->status &= ~I2C_READ_DMA_COMPLETE;
			ret_val = HAL_I2C_Mem_Read_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, I2C_24XX_PAGESIZE-1);
			if ( ret_val )
			{
				i2c_24xx_Drv->status |= I2C_ERROR;
				return ret_val;
			}
			while ( (i2c_24xx_Drv->status & I2C_READ_DMA_COMPLETE ) != I2C_READ_DMA_COMPLETE )
				task_delay(1);
		}
		else
		{
			i2c_24xx_Drv->status &= ~I2C_READ_COMPLETE;
			ret_val =  HAL_I2C_Mem_Read_IT(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, I2C_24XX_PAGESIZE-1);
			if ( ret_val )
			{
				i2c_24xx_Drv->status |= I2C_ERROR;
				return ret_val;
			}
			while ( (i2c_24xx_Drv->status & I2C_READ_COMPLETE ) != I2C_READ_COMPLETE )
				task_delay(1);
		}
		data_len -= I2C_24XX_PAGESIZE;
		address += I2C_24XX_PAGESIZE;
		data += I2C_24XX_PAGESIZE;
	}
	if ( data_len)
	{
		if (( i2c_24xx_Drv->flags & I2C_USES_READ_DMA ) == I2C_USES_READ_DMA)
		{
			i2c_24xx_Drv->status &= ~I2C_READ_DMA_COMPLETE;
			ret_val =  HAL_I2C_Mem_Read_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1);
			if ( ret_val )
			{
				i2c_24xx_Drv->status |= I2C_ERROR;
				return ret_val;
			}
			while ( (i2c_24xx_Drv->status & I2C_READ_DMA_COMPLETE ) != I2C_READ_DMA_COMPLETE )
				task_delay(1);
		}
		else
		{
			i2c_24xx_Drv->status &= ~I2C_READ_COMPLETE;
			ret_val =  HAL_I2C_Mem_Read_IT(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1);
			if ( ret_val )
			{
				i2c_24xx_Drv->status |= I2C_ERROR;
				return ret_val;
			}
			while ( (i2c_24xx_Drv->status & I2C_READ_COMPLETE ) != I2C_READ_COMPLETE )
				task_delay(1);
		}
	}
	if (( i2c_24xx_Drv->flags & I2C_WAKEUP_ON_READ) == I2C_WAKEUP_ON_READ)
		activate_process(ExtFlashDriverStruct[handle].process,i2c_24xx_Drv->wakeup_id,WAKEUP_FLAGS_I2C_RX);
	return ret_val;
}

ITCM_AREA_CODE uint32_t i2c_24xx_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
uint32_t ret_val;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
	while(data_len > I2C_24XX_PAGESIZE)
	{
		if (( i2c_24xx_Drv->flags & I2C_USES_WRITE_DMA ) == I2C_USES_WRITE_DMA)
		{
			i2c_24xx_Drv->status &= ~I2C_WRITE_DMA_COMPLETE;
			ret_val =   HAL_I2C_Mem_Write_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, I2C_24XX_PAGESIZE-1);
			if ( ret_val )
				return ret_val;
			while ( (i2c_24xx_Drv->status & I2C_WRITE_DMA_COMPLETE ) != I2C_WRITE_DMA_COMPLETE )
				task_delay(1);
		}
		else
		{
			i2c_24xx_Drv->status &= ~I2C_WRITE_COMPLETE;
			ret_val =   HAL_I2C_Mem_Write(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, I2C_24XX_PAGESIZE-1, I2C_24XX_TIMEOUT);
			if ( ret_val )
				return ret_val;
			while ( (i2c_24xx_Drv->status & I2C_WRITE_COMPLETE ) != I2C_WRITE_COMPLETE )
				task_delay(1);
		}
		data_len -= I2C_24XX_PAGESIZE;
		address += I2C_24XX_PAGESIZE;
		data += I2C_24XX_PAGESIZE;
	}
	if ( data_len)
	{
		if (( i2c_24xx_Drv->flags & I2C_USES_WRITE_DMA ) == I2C_USES_WRITE_DMA)
		{
			i2c_24xx_Drv->status &= ~I2C_WRITE_DMA_COMPLETE;
			ret_val =   HAL_I2C_Mem_Write_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len);
			if ( ret_val )
				return ret_val;
			while ( (i2c_24xx_Drv->status & I2C_WRITE_DMA_COMPLETE ) != I2C_WRITE_DMA_COMPLETE )
				task_delay(1);
		}
		else
		{
			i2c_24xx_Drv->status &= ~I2C_WRITE_COMPLETE;
			ret_val =   HAL_I2C_Mem_Write(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1, I2C_24XX_TIMEOUT);
			if ( ret_val )
				return ret_val;
			while ( (i2c_24xx_Drv->status & I2C_WRITE_COMPLETE ) != I2C_WRITE_COMPLETE )
				task_delay(1);
		}
	}
	if (( i2c_24xx_Drv->flags & I2C_WAKEUP_ON_WRITE) == I2C_WAKEUP_ON_WRITE)
		activate_process(ExtFlashDriverStruct[handle].process,i2c_24xx_Drv->wakeup_id,WAKEUP_FLAGS_I2C_TX);
	return ret_val;
}

ITCM_AREA_CODE uint32_t i2c_24xx_read_config(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
		return HAL_I2C_Mem_Read(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address | 0x10, address, i2c_24xx_Drv->device_address_size, data, data_len, I2C_24XX_TIMEOUT);
}


ITCM_AREA_CODE uint32_t i2c_24xx_init(uint8_t handle)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[handle].driver_private_data;
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

ITCM_AREA_CODE uint32_t i2c_24xx_eraseblocks(uint8_t handle, uint32_t start_block, uint32_t number_of_blocks)
{
	return 0;
}
ITCM_AREA_CODE uint32_t i2c_24xx_erasechip(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE uint32_t	i2c_24xx_register(I2C_24xx_Drv_TypeDef *driver_private_data)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	if ( ExtFlashDriverStruct[last_extflash_used_handle].process == 0 )
	{
		if ( driver_private_data->wakeup_id == 0 )
			return DRIVER_REQUEST_FAILED;
		ExtFlashDriverStruct[last_extflash_used_handle].process = get_current_process();
		ExtFlashDriverStruct[last_extflash_used_handle].driver_private_data = (uint32_t *)driver_private_data;
		ExtFlashDriverStruct[last_extflash_used_handle].read = i2c_24xx_read;
		ExtFlashDriverStruct[last_extflash_used_handle].write = i2c_24xx_write;
		ExtFlashDriverStruct[last_extflash_used_handle].erase_blocks = i2c_24xx_eraseblocks;
		ExtFlashDriverStruct[last_extflash_used_handle].erase_chip = i2c_24xx_erasechip;
		ExtFlashDriverStruct[last_extflash_used_handle].get_status = i2c_24xx_get_status;
		ExtFlashDriverStruct[last_extflash_used_handle].get_flags = i2c_24xx_get_flags;
		ExtFlashDriverStruct[last_extflash_used_handle].set_flags = i2c_24xx_set_flags;

		i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[last_extflash_used_handle].driver_private_data;

		if ( i2c_24xx_Drv->bus->hdmarx == NULL )
		{
			/* disable dma if they are not configured in hw */
			i2c_24xx_Drv->flags &= ~I2C_USES_READ_DMA;
		}
		if ( i2c_24xx_Drv->bus->hdmatx == NULL )
		{
			/* disable dma if they are not configured in hw */
			i2c_24xx_Drv->flags &= ~I2C_USES_WRITE_DMA;
		}
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
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	for(i=0;i<MAX_I2C_DEVICES;i++)
	{
		if ( ExtFlashDriverStruct[i].process != 0)
		{
			i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[i].driver_private_data;
			if ( i2c_24xx_Drv->bus == hi2c )
			{
				if (( i2c_24xx_Drv->flags & I2C_USES_READ_DMA ) == I2C_USES_READ_DMA)
					i2c_24xx_Drv->status |= I2C_READ_DMA_COMPLETE;
				else
					i2c_24xx_Drv->status |= I2C_READ_COMPLETE;
			}
		}
		else
			return;
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
uint32_t	i;
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	for(i=0;i<MAX_I2C_DEVICES;i++)
	{
		if ( ExtFlashDriverStruct[i].process != 0)
		{
			i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)ExtFlashDriverStruct[i].driver_private_data;
			if ( i2c_24xx_Drv->bus == hi2c )
			{
				if (( i2c_24xx_Drv->flags & I2C_USES_READ_DMA ) == I2C_USES_READ_DMA)
					i2c_24xx_Drv->status |= I2C_WRITE_DMA_COMPLETE;
				else
					i2c_24xx_Drv->status |= I2C_WRITE_COMPLETE;
			}
		}
		else
			return;
	}
}

