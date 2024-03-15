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
 * sht40.c
 *
 *  Created on: Mar 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"

#ifdef SENSORS_SHT40
#include "../../../kernel/A_exported_functions.h"
#include "sht40.h"

uint32_t	sht40_status;

 static int32_t	inline SHT40_acq_start(uint8_t	sht40_reg)
{
	return hw_i2c_Send(SENSORS_SHT40_I2C_INDEX,SHT40_ADDR, &sht40_reg, 1);
}

 int32_t  SHT40_Start_HP_Acquisition(void)
{
	return SHT40_acq_start(SHT40_DATA_HP);
}

 int32_t  SHT40_Start_MP_Acquisition(void)
{
	return SHT40_acq_start(SHT40_DATA_MP);
}

 int32_t  SHT40_Start_LP_Acquisition(void)
{
	return SHT40_acq_start(SHT40_DATA_LP);
}

 int32_t  SHT40_Start_Heat200_1sec_Acquisition(void)
{
	return SHT40_acq_start(SHT40_HEAT200_1);
}

 int32_t  SHT40_Start_Heat200_01sec_Acquisition(void)
{
	return SHT40_acq_start(SHT40_HEAT200_01);
}

 int32_t  SHT40_Start_Heat110_1sec_Acquisition(void)
{
	return SHT40_acq_start(SHT40_HEAT110_1);
}

 int32_t  SHT40_Start_Heat110_01sec_Acquisition(void)
{
	return SHT40_acq_start(SHT40_HEAT110_01);
}

 int32_t  SHT40_Start_Heat20_1sec_Acquisition(void)
{
	return SHT40_acq_start(SHT40_HEAT20_1);
}

 int32_t  SHT40_Start_Heat20_01sec_Acquisition(void)
{
	return SHT40_acq_start(SHT40_HEAT20_01);
}

 int32_t  SHT40_ReadData(uint8_t *pData)
{
	return sht40_status = hw_i2c_Get(SENSORS_SHT40_I2C_INDEX, SHT40_ADDR, pData, 6);
}

 int32_t  SHT40_Get_uid(uint8_t *pData)
{
uint8_t		sht40_reg = SHT40_UUID;
	sht40_status =  hw_i2c_Send(SENSORS_SHT40_I2C_INDEX,SHT40_ADDR, &sht40_reg, 1);
	return sht40_status = hw_i2c_Get(SENSORS_SHT40_I2C_INDEX, SHT40_ADDR, pData, 2);
}

#endif // #ifdef SENSORS_LPS22DF
