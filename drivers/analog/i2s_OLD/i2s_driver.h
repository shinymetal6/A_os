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
 * i2s_driver.h
 *
 *  Created on: Oct 23, 2025
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_I2S_I2S_DRIVER_H_
#define DRIVERS_ANALOG_I2S_I2S_DRIVER_H_
#ifdef SOUND_ENGINE_I2S_ENABLED

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint16_t 			*in_buffer[2];
	uint16_t 			*out_buffer[2];
	I2S_HandleTypeDef 	*i2s;
}I2S_DriverStruct_t;

#define		I2S_BUFFER_SIZE			256
#endif //#ifdef SOUND_ENGINE_I2S_ENABLED

#endif /* DRIVERS_ANALOG_I2S_I2S_DRIVER_H_ */
