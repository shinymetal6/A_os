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
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"

#ifdef A_OS_I2C_ENABLED

#include "i2c_24xx.h"
#include <string.h>

extern	I2C_DriverStruct_t			I2C_DriverStruct[MAX_EXTMEM_DRIVERS];
extern	uint8_t						last_i2c_used_handle;

uint32_t	irq_count=0;

ITCM_AREA_CODE static uint32_t i2c_24xx_seterror(I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv)
{
	i2c_24xx_Drv->status &= ~I2C_STATUS_BUSY;
	i2c_24xx_Drv->status |= I2C_STATUS_ERROR;
	return i2c_24xx_Drv->status;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_get_status(uint8_t handle)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[handle].private_data;
	return i2c_24xx_Drv->status;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_get_flags(uint8_t handle)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[handle].private_data;
	return i2c_24xx_Drv->flags;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_set_flags(uint8_t handle, uint32_t flags)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[handle].private_data;
	i2c_24xx_Drv->flags = flags;
	return  0;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_wait_on_flag_timeout(I2C_24xx_Drv_TypeDef *i2c_24xx_Drv , uint8_t mask)
{
	i2c_24xx_Drv->timeout = I2C_24XX_TIMEOUT;
	while ( (i2c_24xx_Drv->status & mask ) != mask )
	{
		task_delay(10);
		i2c_24xx_Drv->timeout--;
		if ( i2c_24xx_Drv->timeout == 0 )
			return i2c_24xx_seterror(i2c_24xx_Drv);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_wait_device_ready_timeout(I2C_24xx_Drv_TypeDef *i2c_24xx_Drv)
{
	i2c_24xx_Drv->timeout = I2C_24XX_TIMEOUT;
	i2c_24xx_Drv->status |= I2C_STATUS_DEVICE_BUSY;
	while ( (i2c_24xx_Drv->status & I2C_STATUS_DEVICE_BUSY ) == I2C_STATUS_DEVICE_BUSY )
	{
		task_delay(1);
		if ( HAL_I2C_IsDeviceReady(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, 5,I2C_24XX_TIMEOUT) == 0 )
		{
			i2c_24xx_Drv->status &= ~I2C_STATUS_DEVICE_BUSY;
			return 0;
		}
		else
		{
			i2c_24xx_Drv->timeout--;
			if ( i2c_24xx_Drv->timeout == 0 )
				return i2c_24xx_seterror(i2c_24xx_Drv);
		}
	}
	i2c_24xx_Drv->status |= I2C_STATUS_DEVICE_ERROR;
	return 1;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
uint32_t ret_val;
uint32_t p_data_len;

	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[handle].private_data;
	if ( get_and_set_i2cmem_bus_lock(i2c_24xx_Drv->bus,handle) == 0 )
	{
		if ( (address & 0xff ) != 0 )
		{
			if ( ((address & 0xff ) + data_len) <=  I2C_24XX_PAGESIZE)
			{
				i2c_24xx_Drv->status &= ~I2C_STATUS_READ_COMPLETE;
				if (( i2c_24xx_Drv->flags & I2C_FLAGS_USES_READ_DMA ) == I2C_FLAGS_USES_READ_DMA)
					ret_val =  HAL_I2C_Mem_Read_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1);
				else
					ret_val =  HAL_I2C_Mem_Read_IT(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1);
				if ( ret_val )
					return i2c_24xx_seterror(i2c_24xx_Drv);
				data_len = 0;
			}
			else
			{
				p_data_len = I2C_24XX_PAGESIZE - (address & 0xff );
				i2c_24xx_Drv->status &= ~I2C_STATUS_READ_COMPLETE;
				if (( i2c_24xx_Drv->flags & I2C_FLAGS_USES_READ_DMA ) == I2C_FLAGS_USES_READ_DMA)
					ret_val =  HAL_I2C_Mem_Read_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, p_data_len-1);
				else
					ret_val =  HAL_I2C_Mem_Read_IT(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, p_data_len-1);
				if ( ret_val )
					return i2c_24xx_seterror(i2c_24xx_Drv);
				data_len -= p_data_len;
				address &= 0xffffff00;
				address += 0x100;
				data += p_data_len;
			}
			if ( i2c_24xx_wait_on_flag_timeout(i2c_24xx_Drv,I2C_STATUS_READ_COMPLETE) == 1 )
				return i2c_24xx_seterror(i2c_24xx_Drv);
		}
		while(data_len > I2C_24XX_PAGESIZE)
		{
			i2c_24xx_Drv->status &= ~I2C_STATUS_READ_COMPLETE;
			if (( i2c_24xx_Drv->flags & I2C_FLAGS_USES_READ_DMA ) == I2C_FLAGS_USES_READ_DMA)
				ret_val = HAL_I2C_Mem_Read_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, I2C_24XX_PAGESIZE-1);
			else
				ret_val =  HAL_I2C_Mem_Read_IT(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, I2C_24XX_PAGESIZE-1);
			if ( ret_val )
				return i2c_24xx_seterror(i2c_24xx_Drv);

			if ( i2c_24xx_wait_on_flag_timeout(i2c_24xx_Drv,I2C_STATUS_READ_COMPLETE) == 1 )
				return i2c_24xx_seterror(i2c_24xx_Drv);

			data_len -= I2C_24XX_PAGESIZE;
			address += I2C_24XX_PAGESIZE;
			data += I2C_24XX_PAGESIZE;
		}
		if ( data_len)
		{
			i2c_24xx_Drv->status &= ~I2C_STATUS_READ_COMPLETE;
			if (( i2c_24xx_Drv->flags & I2C_FLAGS_USES_READ_DMA ) == I2C_FLAGS_USES_READ_DMA)
				ret_val =  HAL_I2C_Mem_Read_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1);
			else
				ret_val =  HAL_I2C_Mem_Read_IT(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1);
			if ( ret_val )
				return i2c_24xx_seterror(i2c_24xx_Drv);
		}

		if (( i2c_24xx_Drv->flags & I2C_FLAGS_WAIT_ON_READ_COMPLETE ) == I2C_FLAGS_WAIT_ON_READ_COMPLETE)
			if ( i2c_24xx_wait_on_flag_timeout(i2c_24xx_Drv,I2C_STATUS_READ_COMPLETE) == 1 )
				return i2c_24xx_seterror(i2c_24xx_Drv);

		if (( i2c_24xx_Drv->flags & I2C_FLAGS_WAKEUP_ON_READ) == I2C_FLAGS_WAKEUP_ON_READ)
			activate_process(I2C_DriverStruct[handle].process,i2c_24xx_Drv->wakeup_id,WAKEUP_FLAGS_I2C_RX);
		unset_i2cmem_bus_lock(i2c_24xx_Drv->bus,handle);
		return 0;
	}
	else
		return i2c_24xx_seterror(i2c_24xx_Drv);
}


ITCM_AREA_CODE static uint32_t i2c_24xx_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
uint32_t ret_val;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[handle].private_data;
	if ( get_and_set_i2cmem_bus_lock(i2c_24xx_Drv->bus,handle) == 0 )
	{
		while(data_len > I2C_24XX_PAGESIZE)
		{
			i2c_24xx_Drv->status &= ~I2C_STATUS_WRITE_COMPLETE;
			if (( i2c_24xx_Drv->flags & I2C_FLAGS_USES_WRITE_DMA ) == I2C_FLAGS_USES_WRITE_DMA)
				ret_val =   HAL_I2C_Mem_Write_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, I2C_24XX_PAGESIZE-1);
			else
				ret_val =   HAL_I2C_Mem_Write_IT(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, I2C_24XX_PAGESIZE-1);
			if ( ret_val )
				return i2c_24xx_seterror(i2c_24xx_Drv);

			if ( i2c_24xx_wait_on_flag_timeout(i2c_24xx_Drv,I2C_STATUS_WRITE_COMPLETE) == 1 )
				return i2c_24xx_seterror(i2c_24xx_Drv);

			if ( i2c_24xx_wait_device_ready_timeout(i2c_24xx_Drv) )
				return i2c_24xx_seterror(i2c_24xx_Drv);

			data_len -= I2C_24XX_PAGESIZE;
			address += I2C_24XX_PAGESIZE;
			data += I2C_24XX_PAGESIZE;
		}
		if ( data_len)
		{
			i2c_24xx_Drv->status &= ~I2C_STATUS_WRITE_COMPLETE;
			if (( i2c_24xx_Drv->flags & I2C_FLAGS_USES_WRITE_DMA ) == I2C_FLAGS_USES_WRITE_DMA)
				ret_val =   HAL_I2C_Mem_Write_DMA(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len);
			else
				ret_val =   HAL_I2C_Mem_Write_IT(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1);
			if ( ret_val )
				return i2c_24xx_seterror(i2c_24xx_Drv);

			if ( i2c_24xx_wait_on_flag_timeout(i2c_24xx_Drv,I2C_STATUS_WRITE_COMPLETE) == 1 )
				return i2c_24xx_seterror(i2c_24xx_Drv);

			if ( i2c_24xx_wait_device_ready_timeout(i2c_24xx_Drv) )
				return i2c_24xx_seterror(i2c_24xx_Drv);
		}
		if (( i2c_24xx_Drv->flags & I2C_FLAGS_WAIT_ON_WRITE_COMPLETE ) == I2C_FLAGS_WAIT_ON_WRITE_COMPLETE)
			if ( i2c_24xx_wait_on_flag_timeout(i2c_24xx_Drv,I2C_STATUS_WRITE_COMPLETE) == 1 )
				return i2c_24xx_seterror(i2c_24xx_Drv);

		if (( i2c_24xx_Drv->flags & I2C_FLAGS_WAKEUP_ON_WRITE) == I2C_FLAGS_WAKEUP_ON_WRITE)
			activate_process(I2C_DriverStruct[handle].process,i2c_24xx_Drv->wakeup_id,WAKEUP_FLAGS_I2C_TX);
		unset_i2cmem_bus_lock(i2c_24xx_Drv->bus,handle);
		return 0;
	}
	else
		return i2c_24xx_seterror(i2c_24xx_Drv);

}

ITCM_AREA_CODE static uint32_t i2c_24xx_init(uint8_t handle)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[handle].private_data;
	if ( i2c_24xx_Drv->power_port != NULL )
	{
		if ( i2c_24xx_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(i2c_24xx_Drv->power_port, i2c_24xx_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(i2c_24xx_Drv->power_port, i2c_24xx_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_eraseblocks(uint8_t handle, uint32_t start_block, uint32_t number_of_blocks)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_erasechip(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t i2c_24xx_reset(uint8_t handle)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[handle].private_data;
uint8_t	i;
	if ( i2c_24xx_Drv->i2c_scl_port != NULL )
	{
		set_gpio_mode(i2c_24xx_Drv->i2c_scl_port,i2c_24xx_Drv->i2c_scl_bit,MODE_OUTPUT,0);
		for(i=0;i<9;i++)
		{
			  HAL_GPIO_WritePin(i2c_24xx_Drv->i2c_scl_port, i2c_24xx_Drv->i2c_scl_bit, GPIO_PIN_SET);
			  DWT_Delay_us(10);
			  HAL_GPIO_WritePin(i2c_24xx_Drv->i2c_scl_port, i2c_24xx_Drv->i2c_scl_bit, GPIO_PIN_RESET);
			  DWT_Delay_us(10);
		}
		set_gpio_mode(i2c_24xx_Drv->i2c_scl_port,i2c_24xx_Drv->i2c_scl_bit,MODE_AF,0);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t	i2c_24xx_register(I2C_24xx_Drv_TypeDef *private_data)
{
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	if ( I2C_DriverStruct[last_i2c_used_handle].process == 0 )
	{
		if ( private_data->wakeup_id == 0 )
			return DRIVER_REQUEST_FAILED;
		I2C_DriverStruct[last_i2c_used_handle].process = get_current_process();
		I2C_DriverStruct[last_i2c_used_handle].handle = last_i2c_used_handle;
		I2C_DriverStruct[last_i2c_used_handle].private_data = (uint32_t *)private_data;
		I2C_DriverStruct[last_i2c_used_handle].read = i2c_24xx_read;
		I2C_DriverStruct[last_i2c_used_handle].write = i2c_24xx_write;
		I2C_DriverStruct[last_i2c_used_handle].erase_blocks = i2c_24xx_eraseblocks;
		I2C_DriverStruct[last_i2c_used_handle].erase_chip = i2c_24xx_erasechip;
		I2C_DriverStruct[last_i2c_used_handle].get_status = i2c_24xx_get_status;
		I2C_DriverStruct[last_i2c_used_handle].get_flags = i2c_24xx_get_flags;
		I2C_DriverStruct[last_i2c_used_handle].set_flags = i2c_24xx_set_flags;

		i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[last_i2c_used_handle].private_data;
		if ( i2c_24xx_Drv->bus->hdmarx == NULL )
		{
			/* disable dma if they are not configured in hw */
			i2c_24xx_Drv->flags &= ~I2C_FLAGS_USES_READ_DMA;
		}
		if ( i2c_24xx_Drv->bus->hdmatx == NULL )
		{
			/* disable dma if they are not configured in hw */
			i2c_24xx_Drv->flags &= ~I2C_FLAGS_USES_WRITE_DMA;
		}
		I2C_DriverStruct[last_i2c_used_handle].bus = i2c_24xx_Drv->bus;
		i2c_24xx_reset(last_i2c_used_handle);
		if ( HAL_I2C_IsDeviceReady(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, 5,I2C_24XX_TIMEOUT) == 0 )
			i2c_24xx_Drv->status = I2C_STATUS_READY;
		i2c_24xx_init(last_i2c_used_handle);
		I2C_DriverStruct[last_i2c_used_handle].status = DRIVER_STATUS_IN_USE;
		last_i2c_used_handle++;
		return last_i2c_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

/****** Interrupt **********/



#endif // #ifdef A_OS_I2C_ENABLED

