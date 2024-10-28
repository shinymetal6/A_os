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
 * lps22df.c
 *
 *  Created on: Mar 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#include "lps22df.h"
#include <string.h>

extern	DriverStruct_t		*DriverStruct[MAX_DRIVERS];

static uint8_t	read_lps22df_reg(I2C_HandleTypeDef 	*bus,uint16_t device_address,uint8_t internal_address,uint8_t *pData)
{
	if ( HAL_I2C_Mem_Read(bus, device_address, internal_address, 1, pData, 1, LPS22DF_I2C_TIMEOUT) != 0 )
		return 255;
	return pData[0];
}

static uint8_t	write_lps22df_reg(I2C_HandleTypeDef *bus,uint16_t device_address,uint8_t internal_address,uint8_t *pData)
{
	return 	HAL_I2C_Mem_Write(bus, device_address, internal_address, 1, pData, 1, LPS22DF_I2C_TIMEOUT);

}

static uint32_t lps22df_start(uint8_t handle)
{
Lps22df_Drv_TypeDef	*lps22df_Drv;
uint8_t	reg4;

	lps22df_Drv = (Lps22df_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	reg4 = read_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG4,&reg4);
	if (reg4 != 255 )
	{
		reg4 |= LPS22DF_IE_REG4;
		return write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG4,&reg4);
	}
	return 1;
}

static uint32_t lps22df_stop(uint8_t handle)
{
Lps22df_Drv_TypeDef	*lps22df_Drv;
uint8_t	reg4;

	lps22df_Drv = (Lps22df_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	reg4 = read_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG4,&reg4);
	if (reg4 != 255 )
	{
		reg4 &= ~LPS22DF_IE_REG4;
		return write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG4,&reg4);
	}
	return 1;
}

static uint32_t lps22df_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t lps22df_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
Lps22df_Drv_TypeDef	*lps22df_Drv;
	lps22df_Drv = (Lps22df_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	return HAL_I2C_Mem_Read(lps22df_Drv->bus,lps22df_Drv->device_address, (uint16_t )LPS22DF_PRESS_OUT_XL, I2C_MEMADD_SIZE_8BIT, values, values_number, LPS22DF_I2C_TIMEOUT);
}

static uint32_t lps22df_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t lps22df_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	DriverStruct_t	Lps22df_Def_Drv;

uint32_t lps22df_deinit(uint8_t handle)
{
	return driver_unregister(&Lps22df_Def_Drv);
}

static uint32_t lps22df_init(uint8_t handle)
{
Lps22df_Drv_TypeDef	*lps22df_Drv;
uint8_t	reg_c;
	lps22df_Drv = (Lps22df_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	lps22df_Drv->status = LPS22DF_STOPPED;
	if ( lps22df_Drv->power_port != NULL )
	{
		if ( lps22df_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_RESET);
	}
	if ( read_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_WHO_AM_I,&lps22df_Drv->whoami) == LPS22DF_ID)
	{
		if ( lps22df_Drv->flags == LPS22DF_DRIVER_DEFAULT)
		{
			reg_c = LPS22DF_RESET_REG2;
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG2,&reg_c);
			reg_c = LPS22DF_BOOT_REG2;
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG2,&reg_c);
			reg_c = LPS22DF_ONE_SHOT_REG2 | LPS22DF_LPF_REG2;
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG2,&reg_c);
			reg_c = LPS22DF_ONE_SHOT | LPS22DF_16_AVG;
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG1,&reg_c);
			reg_c = LPS22DF_IE_REG4;
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG4,&reg_c);
		}
		else
		{
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_THS_P_L,&lps22df_Drv->ths_p_l);
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_THS_P_H,&lps22df_Drv->ths_p_h);
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG1,&lps22df_Drv->ctl_reg1);
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG2,&lps22df_Drv->ctl_reg2);
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG3,&lps22df_Drv->ctl_reg3);
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_CTRL_REG4,&lps22df_Drv->ctl_reg4);
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_FIFO_CTRL,&lps22df_Drv->fifo_ctl);
			write_lps22df_reg(lps22df_Drv->bus,lps22df_Drv->device_address,LPS22DF_FIFO_WTM,&lps22df_Drv->fifo_wtm);
		}
		return 0;
	}
	return 1;
}

DriverStruct_t	Lps22df_Def_Drv =
{
	.init = lps22df_init,
	.deinit = lps22df_deinit,
	.start = lps22df_start,
	.stop = lps22df_stop,
	.extended_action = lps22df_extended_actions,
	.get_status = lps22df_get_status,
	.get_values = lps22df_get_values,
	.set_values = lps22df_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "lps22df",
};

uint32_t lps22df_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&Lps22df_Def_Drv,sizeof(Lps22df_Def_Drv));
	return 0;
}



