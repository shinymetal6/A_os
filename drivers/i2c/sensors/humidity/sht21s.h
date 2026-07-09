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
 * sht21s.h
 *
 *  Created on: Jul 9, 2026
 *      Author: fil
 */

#ifndef DRIVERS_I2C_SENSORS_HUMIDITY_SHT21S_H_
#define DRIVERS_I2C_SENSORS_HUMIDITY_SHT21S_H_


typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint32_t 			wakeup_id;
	I2C_DriverStruct_t	*next_drv;
	/* driver proprietary data */
	uint8_t				device_flags;
	uint32_t 			device_config;
	uint32_t			sensor_id;
	GPIO_TypeDef	 	*i2c_scl_port;
	uint16_t			i2c_scl_bit;
	uint8_t 			device_address_size;
	uint32_t 			device_size;
	uint32_t 			timeout;
	uint32_t			who_am_i;
	uint8_t				*data;
	uint8_t				*additional_data;
	float 				temperature,humidity;
}I2C_Sht21s_DriverStruct_t;

#define SHT21S_I2C_ADDR          0x80
#define SHT21S_I2C_TIMEOUT		1000U

// --- SHT21 Commands ---
#define SHT21_CMD_TRIG_TEMP_NHM 0xF3
#define SHT21_CMD_TRIG_HUM_NHM  0xF5
#define SHT21_CMD_SOFT_RESET    0xFE


#endif /* DRIVERS_I2C_SENSORS_HUMIDITY_SHT21S_H_ */
