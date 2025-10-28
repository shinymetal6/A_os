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
 *  Created on: Oct 24, 2025
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
	uint32_t			*pre_drv;
	uint32_t			*next_drv;
	uint16_t 			*i2s_rx_buffer;
	uint16_t 			*i2s_tx_buffer;
	uint16_t 			*left_rx_buffer;
	uint16_t 			*right_rx_buffer;
	uint16_t 			*left_tx_buffer;
	uint16_t 			*right_tx_buffer;
	I2S_HandleTypeDef 	*i2s;
}I2S_DriverStruct_t;
/* flags */
#define		I2S_FLAGS_ECHO			0x80

#define		I2S_BUFFER_SIZE			2048
#define		I2S_HALF_BUFFER_SIZE	(I2S_BUFFER_SIZE/2)
#define		I2S_LEFT_CHANNEL		0
#define		I2S_RIGHT_CHANNEL		1
#define		I2S_FIRST_HALF			0
#define		I2S_SECOND_HALF			(I2S_BUFFER_SIZE/2)
#define		I2S_EFFECT_SIZE			(I2S_BUFFER_SIZE/16)
#define		I2S_SAMPLE_FREQUENCY	48000

extern uint32_t	i2s_driver_register(I2S_DriverStruct_t *i2s);
extern uint32_t	i2s_driver_start(I2S_DriverStruct_t *i2s);

#endif //#ifdef SOUND_ENGINE_I2S_ENABLED

#endif /* DRIVERS_ANALOG_I2S_I2S_DRIVER_H_ */
