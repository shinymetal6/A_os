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

#ifdef SENSORS_LPS22DF
#include "../../../kernel/A_exported_functions.h"
#include "lps22df.h"

 static uint8_t	read_lps22df_reg(uint8_t address)
{
uint8_t	rreg;
	hw_i2c_MemGet8(SENSORS_LPS22DF_I2C_INDEX,LPS22DF_ADDR,(uint16_t )address,&rreg, 1);
	return rreg;
}

static uint8_t	write_lps22df_reg(uint8_t address,uint8_t data)
{
	return hw_i2c_MemSend8(SENSORS_LPS22DF_I2C_INDEX,LPS22DF_ADDR, (uint16_t )address,&data, 1);
}

 uint8_t LPS22DF_GetWhoAmI(void)
{
	read_lps22df_reg(LPS22DF_WHO_AM_I);
	return read_lps22df_reg(LPS22DF_WHO_AM_I);
}

 uint8_t LPS22DF_GetStatus(void)
{
	return read_lps22df_reg(LPS22DF_STATUS);
}

 uint8_t LPS22DF_Read_P_Data(uint8_t *pressure_p_ptr)
{
	return hw_i2c_MemGet8(SENSORS_LPS22DF_I2C_INDEX,LPS22DF_ADDR,(uint16_t )LPS22DF_PRESS_OUT_XL,pressure_p_ptr, LPS22DF_P_LEN);
}

 uint8_t LPS22DF_Read_T_Data(uint8_t *pressure_t_ptr)
{
	return hw_i2c_MemGet8(SENSORS_LPS22DF_I2C_INDEX,LPS22DF_ADDR,(uint16_t )LPS22DF_TEMP_OUT_L,pressure_t_ptr, LPS22DF_T_LEN);
}

 int32_t LPS22DF_Start_Acquisition(void)
{
	return write_lps22df_reg(LPS22DF_CTRL_REG2,LPS22DF_ONE_SHOT_REG2);
}

 uint32_t LPS22DF_Init(void)
{
	write_lps22df_reg(LPS22DF_CTRL_REG2,LPS22DF_RESET_REG2);
	write_lps22df_reg(LPS22DF_CTRL_REG2,LPS22DF_BOOT_REG2);
	while((read_lps22df_reg(LPS22DF_INT_SOURCE) & LPS22DF_BOOT_ON_REG_INT_SOURCE) == LPS22DF_BOOT_ON_REG_INT_SOURCE);
	write_lps22df_reg(LPS22DF_CTRL_REG2,LPS22DF_ONE_SHOT_REG2 | LPS22DF_LPF_REG2);
	write_lps22df_reg(LPS22DF_CTRL_REG1,(LPS22DF_ONE_SHOT | LPS22DF_16_AVG));
	write_lps22df_reg(LPS22DF_CTRL_REG4,LPS22DF_IE_REG4);
	return 0;
}

#endif // #ifdef SENSORS_LPS22DF

