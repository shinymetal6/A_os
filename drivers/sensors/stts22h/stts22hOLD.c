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
 * stts22h.c
 *
 *  Created on: Mar 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"

#ifdef SENSORS_STTS22H
#include "../../../kernel/A_exported_functions.h"
#include "stts22h.h"

 static uint8_t	read_stts22h_reg(uint8_t address)
{
uint8_t	rreg;
	hw_i2c_MemGet8(SENSORS_STTS22H_I2C_INDEX,STTS22H_ADDR,(uint16_t )address,&rreg, 1);
	return rreg;
}

 static uint8_t	write_stts22h_reg(uint8_t address,uint8_t data)
{
	return hw_i2c_MemSend8(SENSORS_STTS22H_I2C_INDEX,STTS22H_ADDR, (uint16_t )address,&data, 1);
}

 uint8_t STTS22H_GetWhoAmI(void)
{
	read_stts22h_reg(STTS22H_WHOAMI);
	return read_stts22h_reg(STTS22H_WHOAMI);
}

 int32_t STTS22H_Start_Acquisition(void)
{
	return write_stts22h_reg(STTS22H_CTRL,STTS22H_ONE_SHOT_REG_CTRL);
}

 uint8_t STTS22H_Read_T_Data(uint8_t *temperature_ptr)
{
	return hw_i2c_MemGet8(SENSORS_STTS22H_I2C_INDEX,STTS22H_ADDR,(uint16_t )STTS22H_TEMP_L_OUT,temperature_ptr, STTS22H_T_LEN);
}

 uint32_t STTS22H_Init(void)
{
	return (uint32_t )STTS22H_GetWhoAmI();
}
#endif // #ifdef SENSORS_STTS22H
