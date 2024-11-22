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
 * int_i2s_driver.h
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_INT_I2S_DRIVER_INT_I2S_DRIVER_H_
#define DRIVERS_ANALOG_INT_I2S_DRIVER_INT_I2S_DRIVER_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	I2S_HandleTypeDef 	*i2s;
	uint8_t 			audio_flags;
	uint8_t 			control_flags;
	float				master_volume;
	int16_t 			*audio_in_buffer;
	int16_t 			*audio_out_buffer;
}I2S_Drv_TypeDef;

extern 	I2S_HandleTypeDef hi2s2;
extern	uint32_t	i2s_register(I2S_Drv_TypeDef *analog_driver_private_data,uint32_t driver_flags);

#endif /* DRIVERS_ANALOG_INT_I2S_DRIVER_INT_I2S_DRIVER_H_ */
