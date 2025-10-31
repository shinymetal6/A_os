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
 *  Created on: Oct 31, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "lps22df.h"

ITCM_AREA_CODE	static uint8_t	read_lps22df_reg(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv,uint8_t address)
{
uint8_t	data;
	if ( HAL_I2C_Mem_Read(lps22df_Drv->bus, lps22df_Drv->device_address, address, lps22df_Drv->device_address_size, &data, 1,LPS22DF_I2C_TIMEOUT) == 0)
		return data;
	return 0;
}

ITCM_AREA_CODE	static uint8_t	write_lps22df_reg(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv,uint8_t address,uint8_t data)
{
	return HAL_I2C_Mem_Write(lps22df_Drv->bus, lps22df_Drv->device_address, address, lps22df_Drv->device_address_size, &data, 1,LPS22DF_I2C_TIMEOUT);
}

ITCM_AREA_CODE uint32_t lps22df_start(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv)
{
	lps22df_Drv->status = LPS22DF_STARTED;
	if ( write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG2,LPS22DF_ONE_SHOT_REG2) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t lps22df_stop(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv)
{
	return 0;
}

ITCM_AREA_CODE uint32_t lps22df_get_data(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv)
{
	if ( HAL_I2C_Master_Receive(lps22df_Drv->bus,lps22df_Drv->device_address, lps22df_Drv->data,LPS22DF_P_LEN, LPS22DF_I2C_TIMEOUT) )
		return 1;
	return 0;
}

ITCM_AREA_CODE uint32_t lps22df_init(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv)
{
	if ( lps22df_Drv->power_port != NULL )
	{
		if ( lps22df_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_RESET);
	}
	lps22df_Drv->who_am_i = read_lps22df_reg(lps22df_Drv,LPS22DF_WHO_AM_I);

	write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG2,LPS22DF_RESET_REG2);
	write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG2,LPS22DF_BOOT_REG2);
	while((read_lps22df_reg(lps22df_Drv,LPS22DF_INT_SOURCE) & LPS22DF_BOOT_ON_REG_INT_SOURCE) == LPS22DF_BOOT_ON_REG_INT_SOURCE)
		task_delay(1);
	write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG2,LPS22DF_ONE_SHOT_REG2 | LPS22DF_LPF_REG2);
	write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG1,(LPS22DF_ONE_SHOT | LPS22DF_16_AVG));
	write_lps22df_reg(lps22df_Drv,LPS22DF_CTRL_REG4,LPS22DF_IE_REG4);
	return 0;
}

ITCM_AREA_CODE uint32_t lps22df_power_on(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv)
{
	if ( lps22df_Drv->power_port != NULL )
	{
		if ( lps22df_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t lps22df_power_off(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv)
{
	if ( lps22df_Drv->power_port != NULL )
	{
		if ( lps22df_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_RESET);
		else
			  HAL_GPIO_WritePin(lps22df_Drv->power_port, lps22df_Drv->power_bit, GPIO_PIN_SET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t lps22df_register(I2C_Lps22DF_Drv_TypeDef *lps22df_Drv)
{

I2C_DriverStruct_t *eptr, *pre_eptr;
	if ( lps22df_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)lps22df_Drv;
		lps22df_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)lps22df_Drv;
		lps22df_Drv->next_drv = NULL;
	}
	lps22df_Drv->process = get_current_process();
	return 0;
}
#endif // #ifdef A_OS_I2C_ENABLED


