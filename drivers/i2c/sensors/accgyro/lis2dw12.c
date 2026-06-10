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
 * lis2dw12.c
 *
 *  Created on: Oct 31, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED
#include "../../i2c.h"
#include "lis2dw12.h"

ITCM_AREA_CODE static uint8_t	write_lis_reg(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv,uint8_t address,uint8_t data)
{
	return HAL_I2C_Mem_Write(lis2dw12_Drv->bus, lis2dw12_Drv->device_address, address, 1, &data, 1,LIS2DW12_I2C_TIMEOUT);
}

ITCM_AREA_CODE static uint8_t	read_lis_reg(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv,uint8_t address,uint8_t *data)
{
	return HAL_I2C_Mem_Read(lis2dw12_Drv->bus, lis2dw12_Drv->device_address, address, 1, data, 1,LIS2DW12_I2C_TIMEOUT);
}

ITCM_AREA_CODE uint32_t lis2dw12_start(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv)
{

	lis2dw12_Drv->status = LIS2DW12_STARTED;
	read_lis_reg( lis2dw12_Drv,LIS2DW12_WHO_AM_I,(uint8_t *)&lis2dw12_Drv->who_am_i);
	write_lis_reg( lis2dw12_Drv, LIS2DW12_CTRL1 , LIS2DW12_LP_MODE_1 | LIS2DW12_MODE_HIGH_PERF | LIS2DW12_ODR_100Hz );
	write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL6, LIS2DW12_BW_FILT_ODR20 | LIS2DW12_FS_16G);
	write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL2 , LIS2DW12_BOOT | LIS2DW12_BDU | LIS2DW12_IF_ADD_INC);
	write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL3 , 0x0);	// pulsed irq

	if (( lis2dw12_Drv->device_flags & IRQ_MODE_ON_WAKEUP ) == IRQ_MODE_ON_WAKEUP )
		write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL4_INT1_PAD_CTRL , LIS2DW12_INT1_WU);
	else
		write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL4_INT1_PAD_CTRL , LIS2DW12_INT1_FTH);

	write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL5_INT2_PAD_CTRL , 0);
	write_lis_reg( lis2dw12_Drv,LIS2DW12_WAKE_UP_THS ,  LIS2DW12_SINGLE_DOUBLE_TAP | 16);
	write_lis_reg( lis2dw12_Drv,LIS2DW12_WAKE_UP_DUR ,  0x10);
	write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL_REG7, LIS2DW12_DRDY_PULSED | LIS2DW12_INTERRUPTS_ENABLE | LIS2DW12_INT2_ON_INT1);

	write_lis_reg( lis2dw12_Drv,LIS2DW12_FIFO_CTRL, LISDW_FIFOMODE_BYPASS << 5 | (lis2dw12_Drv->device_config-1) );
	write_lis_reg( lis2dw12_Drv,LIS2DW12_FIFO_CTRL, LISDW_FIFOMODE_CONTINUOUS << 5 | (lis2dw12_Drv->device_config-1) );

	read_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL4_INT1_PAD_CTRL,&lis2dw12_Drv->status); // dummy read
	read_lis_reg( lis2dw12_Drv,LIS2DW12_STATUS,&lis2dw12_Drv->status);
	return 0;
}

ITCM_AREA_CODE uint32_t lis2dw12_stop(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv)
{
uint8_t temp;
	lis2dw12_Drv->status = LIS2DW12_STOPPED;
	read_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL4_INT1_PAD_CTRL,&temp);
	write_lis_reg( lis2dw12_Drv,LIS2DW12_CTRL2 , temp | 0x40);
	read_lis_reg( lis2dw12_Drv,LIS2DW12_STATUS,&lis2dw12_Drv->status);
	return 0;
}

ITCM_AREA_CODE uint32_t lis2dw12_get_Pdata(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv)
{
	return HAL_I2C_Mem_Read(lis2dw12_Drv->bus, lis2dw12_Drv->device_address, LIS2DW12_OUT_X_L, 1, lis2dw12_Drv->data, MAG_LEN,LIS2DW12_I2C_TIMEOUT);
}

ITCM_AREA_CODE uint32_t lis2dw12_get_Tdata(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv)
{
	return HAL_I2C_Mem_Read(lis2dw12_Drv->bus, lis2dw12_Drv->device_address, LIS2DW12_OUT_T, 1, lis2dw12_Drv->additional_data, 1,LIS2DW12_I2C_TIMEOUT);
}

ITCM_AREA_CODE uint32_t lis2dw12_power_on(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv)
{
	if ( lis2dw12_Drv->power_port != NULL )
	{
		if ( lis2dw12_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_SET);
		else
			  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_RESET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t lis2dw12_power_off(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv)
{
	if ( lis2dw12_Drv->power_port != NULL )
	{
		if ( lis2dw12_Drv->power_active_level == 1 )
			  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_RESET);
		else
			  HAL_GPIO_WritePin(lis2dw12_Drv->power_port, lis2dw12_Drv->power_bit, GPIO_PIN_SET);
	}
	return 0;
}

ITCM_AREA_CODE uint32_t lis2dw12_init(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv)
{
	return lis2dw12_power_on(lis2dw12_Drv);
}

ITCM_AREA_CODE uint32_t lis2dw12_register(I2C_Lis2DW12_DriverStruct_t *lis2dw12_Drv)
{
I2C_DriverStruct_t *eptr, *pre_eptr;
	if ( lis2dw12_Drv->bus == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( i2c_drv_ptr == NULL)
	{
		i2c_drv_ptr = (I2C_DriverStruct_t *)lis2dw12_Drv;
		lis2dw12_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = i2c_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (I2C_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)lis2dw12_Drv;
		lis2dw12_Drv->next_drv = NULL;
	}
	lis2dw12_Drv->process = get_current_process();
	return 0;
}

#endif // #ifdef A_OS_I2C_ENABLED



