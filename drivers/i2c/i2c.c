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

static void i2c_irq_common(I2C_HandleTypeDef *hi2c,uint8_t flags)
{
uint32_t	i;
	for(i=0;i<MAX_I2C_DEVICES;i++)
	{
		if ( I2C_DriverStruct[i].bus != NULL )
		{
			if ( I2C_DriverStruct[i].bus  == hi2c )
			{
				I2C_DriverStruct_t	*i2c_Drv = (I2C_DriverStruct_t *)I2C_DriverStruct[i].private_data;
				i2c_Drv->flags  |= flags;
			}
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

#endif // #ifdef A_OS_I2C_ENABLED
