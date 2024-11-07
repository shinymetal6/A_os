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
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../kernel/kernel_opt.h"


extern		MemDriverStruct_t	*MemDriverStruct[MAX_MEM_DRIVERS];

#include "i2c_24xx.h"
#include <string.h>

static uint32_t i2c_24xx_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t i2c_24xx_read(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef	*)MemDriverStruct[handle]->driver_private_data;
	return HAL_I2C_Mem_Read(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1, I2C_24XX_TIMEOUT);
}

static uint32_t i2c_24xx_read_config(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef	*)MemDriverStruct[handle]->driver_private_data;
	return HAL_I2C_Mem_Read(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address | 0x10, address, i2c_24xx_Drv->device_address_size, data, data_len, I2C_24XX_TIMEOUT);
}

static uint32_t i2c_24xx_write(uint8_t handle, uint32_t address,uint8_t *data,uint16_t data_len)
{
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef	*)MemDriverStruct[handle]->driver_private_data;
	return HAL_I2C_Mem_Write(i2c_24xx_Drv->bus, i2c_24xx_Drv->device_address, address, i2c_24xx_Drv->device_address_size, data, data_len-1, I2C_24XX_TIMEOUT);
}

extern	MemDriverStruct_t	I2C_24xx_Drv;

uint32_t i2c_24xx_deinit(uint8_t handle)
{
	return extmem_driver_unregister(&I2C_24xx_Drv);
}

static uint32_t i2c_24xx_init(uint8_t handle)
{
i2c_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	i2c_24xx_Drv = (i2c_24xx_Drv_TypeDef	*)MemDriverStruct[handle]->driver_private_data;
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

MemDriverStruct_t	I2C_24xx_Drv =
{
	.init = i2c_24xx_init,
	.deinit = i2c_24xx_deinit,
	.get_status = i2c_24xx_get_status,
	.read = i2c_24xx_read,
	.read_config = i2c_24xx_read_config,
	.write = i2c_24xx_write,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "i2c_24xx",
};

uint32_t i2c_24xx_allocate_driver(MemDriverStruct_t *new_struct)
{
	memcpy(new_struct,&I2C_24xx_Drv,sizeof(I2C_24xx_Drv));
	return 0;
}


