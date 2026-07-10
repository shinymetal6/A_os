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
 *  Created on: Oct 28, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED

#include "i2c.h"
I2C_DriverStruct_t	*i2c_drv_ptr;

ITCM_AREA_CODE static void i2c_irq_common(I2C_HandleTypeDef *hi2c,uint32_t flag)
{
I2C_DriverStruct_t	*i2c_drv_ptr_L = i2c_drv_ptr;
	while(i2c_drv_ptr_L->bus != hi2c)
	{
		if ( i2c_drv_ptr_L->next_drv != NULL )
			i2c_drv_ptr_L = (I2C_DriverStruct_t *)i2c_drv_ptr->next_drv;
	}
	if (i2c_drv_ptr_L != NULL)
	{
		if ( i2c_drv_ptr_L->process != 0 )
			i2c_drv_ptr_L->flags |= flag;
	}
	if ( i2c_drv_ptr_L->wakeup_id )
		activate_process(i2c_drv_ptr_L->process,i2c_drv_ptr_L->wakeup_id,flag);

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
	i2c_irq_common(hi2c,I2C_STATUS_READ_COMPLETE);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2c_irq_common(hi2c,I2C_STATUS_WRITE_COMPLETE);
}
#endif //#ifdef A_OS_I2C_ENABLED

