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
 * i2c.c
 *
 *  Created on: Dec 21, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"

#include "i2c.h"
#include <string.h>
#ifdef A_OS_I2C_ENABLED

SYSTEM_RAM	I2C_DriverStruct_t	I2C_DriverStruct[MAX_I2C_DEVICES];
SYSTEM_RAM	uint8_t				last_i2c_used_handle,i2c_driver_request;
SYSTEM_RAM	uint16_t			i2c_generic_arb_var[MAX_I2C_BUSES];
SYSTEM_RAM	uint16_t			i2c_memory_arb_var[MAX_I2C_BUSES];

static uint8_t get_i2c_bus_index(I2C_HandleTypeDef *hi2c)
{
#ifdef I2C1
	if ( hi2c->Instance == I2C1 )
		return 1;
#endif
#ifdef I2C2
	if ( hi2c->Instance == I2C2 )
		return 2;
#endif
#ifdef I2C3
	if ( hi2c->Instance == I2C3 )
		return 3;
#endif
#ifdef I2C4
	if ( hi2c->Instance == I2C4 )
		return 4;
#endif
	return 0;
}

uint8_t get_and_set_i2c_bus_lock(I2C_HandleTypeDef *hi2c,uint8_t handle)
{
uint32_t	irq_state = __get_PRIMASK();// __get_PRIMASK returns 0 if irqs enabled
uint8_t		bus_index = get_i2c_bus_index(hi2c);
uint8_t		i2c_timeout = 100;
	if ( irq_state == 0 )
	{
		__disable_irq();
	}
	if (i2c_generic_arb_var[bus_index] )
	{
		task_delay(1);
		i2c_timeout--;
		if ( i2c_timeout == 0 )
		{
			if ( irq_state == 0 )
			{
				__enable_irq();
			}
			return 1;
		}
	}
	i2c_generic_arb_var[bus_index] |= 1 << handle;
	if ( irq_state == 0 )
	{
		__enable_irq();
	}
	return 0;
}

uint8_t unset_i2c_bus_lock(I2C_HandleTypeDef *hi2c,uint8_t handle)
{
uint32_t	irq_state = __get_PRIMASK();// __get_PRIMASK returns 0 if irqs enabled
uint8_t		bus_index = get_i2c_bus_index(hi2c);
	if ( irq_state == 0 )
	{
		__disable_irq();
	}
	i2c_generic_arb_var[bus_index] &= ~(1 << handle);
	if ( irq_state == 0 )
	{
		__enable_irq();
	}
	return 0;
}

uint8_t get_and_set_i2cmem_bus_lock(I2C_HandleTypeDef *hi2c,uint8_t handle)
{
uint32_t	irq_state = __get_PRIMASK();// __get_PRIMASK returns 0 if irqs enabled
uint8_t		bus_index = get_i2c_bus_index(hi2c);
uint8_t		i2c_timeout = 100;
	if ( irq_state == 0 )
	{
		__disable_irq();
	}
	if (i2c_memory_arb_var[bus_index] )
	{
		task_delay(1);
		i2c_timeout--;
		if ( i2c_timeout == 0 )
		{
			if ( irq_state == 0 )
			{
				__enable_irq();
			}
			return 1;
		}
	}
	i2c_memory_arb_var[bus_index] |= 1 << handle;
	if ( irq_state == 0 )
	{
		__enable_irq();
	}
	return 0;
}

uint8_t unset_i2cmem_bus_lock(I2C_HandleTypeDef *hi2c,uint8_t handle)
{
uint32_t	irq_state = __get_PRIMASK();// __get_PRIMASK returns 0 if irqs enabled
uint8_t		bus_index = get_i2c_bus_index(hi2c);
	if ( irq_state == 0 )
	{
		__disable_irq();
	}
	i2c_memory_arb_var[bus_index] &= ~(1 << handle);
	if ( irq_state == 0 )
	{
		__enable_irq();
	}
	return 0;
}

static void i2c_irq_common(I2C_HandleTypeDef *hi2c,uint8_t flags)
{
uint32_t	i;
	for(i=0;i<MAX_I2C_DEVICES;i++)
	{
		if ( I2C_DriverStruct[i].bus == hi2c )
		{
			I2C_DriverStruct_t	*i2c_Drv = (I2C_DriverStruct_t *)I2C_DriverStruct[i].private_data;
			i2c_Drv->flags  |= flags;
		}
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2c_irq_common(hi2c,I2C_STATUS_READ_COMPLETE);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2c_irq_common(hi2c,I2C_STATUS_WRITE_COMPLETE);
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
uint32_t	i;
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	for(i=0;i<MAX_I2C_DEVICES;i++)
	{
		if ( I2C_DriverStruct[i].process != 0)
		{
			i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[i].private_data;
			if ( i2c_24xx_Drv->bus == hi2c )
				i2c_24xx_Drv->status |= I2C_STATUS_READ_COMPLETE;
		}
		else
			break;
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
uint32_t	i;
I2C_24xx_Drv_TypeDef	*i2c_24xx_Drv;
	for(i=0;i<MAX_I2C_DEVICES;i++)
	{
		if ( I2C_DriverStruct[i].process != 0)
		{
			i2c_24xx_Drv = (I2C_24xx_Drv_TypeDef *)I2C_DriverStruct[i].private_data;
			__DSB ();
			if ( i2c_24xx_Drv->bus == hi2c )
				i2c_24xx_Drv->status |= I2C_STATUS_WRITE_COMPLETE;
		}
		else
			break;
	}
}
#endif // #ifdef A_OS_I2C_ENABLED
