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
 * hw_i2c.c
 *
 *  Created on: Mar 4, 2024
 *      Author: fil
 */

#include "main.h"
#include "../system_default.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../kernel_opt.h"
#include "hw_i2c.h"

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;

#ifdef A_HAS_I2C_BUS
extern	HW_I2C_t	HW_I2C[A_MAX_I2C];

uint8_t hw_i2c_check_presence(uint32_t i2c,uint16_t device_address)
{
	return HAL_I2C_IsDeviceReady(HW_I2C[i2c-HW_I2C1].hwi2c_handle, device_address, I2C_RETRY_CHECK, HW_I2C_TIMEOUT);
}

int32_t hw_i2c_Send(uint32_t i2c,uint16_t device_address,uint8_t *pData, uint16_t Length)
{
	if ( HWMngr[i2c].process != Asys.current_process )
		return HW_I2C_ERROR;
	if (pData == NULL)
		return HW_I2C_ERROR;
	return HAL_I2C_Master_Transmit(HW_I2C[i2c-HW_I2C1].hwi2c_handle, device_address, pData, Length, HW_I2C_TIMEOUT);
}

int32_t hw_i2c_Get(uint32_t i2c,uint16_t device_address,uint8_t *pData, uint16_t Length)
{
	if ( HWMngr[i2c].process != Asys.current_process )
		return HW_I2C_ERROR;
	if (pData == NULL)
		return HW_I2C_ERROR;
	return HAL_I2C_Master_Receive(HW_I2C[i2c-HW_I2C1].hwi2c_handle, device_address, pData, Length, HW_I2C_TIMEOUT);
}

int32_t hw_i2c_MemSend8(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length)
{
	if ( HWMngr[i2c].process != Asys.current_process )
		return HW_I2C_ERROR;
	if (pData == NULL)
		return HW_I2C_ERROR;
	return 	HAL_I2C_Mem_Write(HW_I2C[i2c-HW_I2C1].hwi2c_handle, device_address, internal_address, 1, pData, Length, HW_I2C_TIMEOUT);
}

int32_t hw_i2c_MemSend16(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length)
{
	if ( HWMngr[i2c].process != Asys.current_process )
		return HW_I2C_ERROR;
	if (pData == NULL)
		return HW_I2C_ERROR;
	return 	HAL_I2C_Mem_Write(HW_I2C[i2c-HW_I2C1].hwi2c_handle, device_address, internal_address, 2, pData, Length, HW_I2C_TIMEOUT);
}

int32_t hw_i2c_MemGet8(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length)
{
	if ( HWMngr[i2c].process != Asys.current_process )
		return HW_I2C_ERROR;
	if (pData == NULL)
		return HW_I2C_ERROR;
	return 	HAL_I2C_Mem_Read(HW_I2C[i2c-HW_I2C1].hwi2c_handle, device_address, internal_address, 1, pData, Length, HW_I2C_TIMEOUT);
}

int32_t hw_i2c_MemGet16(uint32_t i2c,uint16_t device_address,uint16_t internal_address,uint8_t *pData, uint16_t Length)
{
	if ( HWMngr[i2c].process != Asys.current_process )
		return HW_I2C_ERROR;
	if (pData == NULL)
		return HW_I2C_ERROR;
	return 	HAL_I2C_Mem_Read(HW_I2C[i2c-HW_I2C1].hwi2c_handle, device_address, internal_address, 2, pData, Length, HW_I2C_TIMEOUT);
}

#endif

void A_hw_i2c_init(void)
{
#ifdef	A_HAS_I2C1
	HW_I2C[0].hwi2c_handle = &hi2c1;
	HW_I2C[0].hwi2c_index  = HW_I2C1;
	HW_I2C[0].hwi2c_flags  = 0;
#endif
#ifdef	A_HAS_I2C2
	HW_I2C[1].hwi2c_handle = &hi2c2;
	HW_I2C[1].hwi2c_index  = HW_I2C2;
	HW_I2C[1].hwi2c_flags  = 0;
#endif
}
