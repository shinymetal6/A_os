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
 * hdc1080c.c
 *
 *  Created on: Jan 22, 2024
 *      Author: Luigi F
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/HwDevices/hw_i2c.h"


#ifdef SENSORS_CCS8118

#include "ccs811.h"
#include "hdc1080.h"


void hdc1080_init(Temp_Reso Temperature_Resolution_x_bit,Humi_Reso Humidity_Resolution_x_bit)
{
	/* Temperature and Humidity are acquired in sequence, Temperature first
	 * Default:   Temperature resolution = 14 bit,
	 *            Humidity resolution    = 14 bit
	 */

	/* Set the acquisition mode to measure both temperature and humidity by setting Bit[12] to 1 */
	uint16_t config_reg_value=0x1000;
	uint8_t data_send[2];

	if(Temperature_Resolution_x_bit == Temperature_Resolution_11_bit)
	{
		config_reg_value |= (1<<10); //11 bit
	}

	switch(Humidity_Resolution_x_bit)
	{
		case Humidity_Resolution_11_bit:
			config_reg_value|= (1<<8);
			break;
		case Humidity_Resolution_8_bit:
			config_reg_value|= (1<<9);
			break;
		default:
			break;
	}

	data_send[0]= (config_reg_value>>8);
	data_send[1]= (config_reg_value&0x00ff);
	
	hw_i2c_MemSend8(SENSORS_CCS818_I2C_INDEX,HDC_1080_ADD<<1, Configuration_register_add, data_send, 2);
	
}


uint8_t hdc1080_start_measurement(float* temperature, float* humidity)
{
	uint8_t receive_data[4];
	uint16_t temp_x,humi_x;
	uint8_t send_data = Temperature_register_add;

	hw_i2c_Send(SENSORS_CCS818_I2C_INDEX, HDC_1080_ADD<<1, &send_data, 1);

	/* Delay here 15ms for conversion compelete.
	 * Note: datasheet say maximum is 7ms, but when delay=7ms, the read value is not correct
	 */
	HAL_Delay(15);

	/* Read temperature and humidity */
	hw_i2c_Get(SENSORS_CCS818_I2C_INDEX,HDC_1080_ADD<<1,receive_data,4);

	temp_x =((receive_data[0]<<8)|receive_data[1]);
	humi_x =((receive_data[2]<<8)|receive_data[3]);

	*temperature=((temp_x/65536.0)*165.0)-40.0;
	*humidity=(humi_x/65536.0)*100.0;

	return 0;

}

#endif


