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
	uint16_t 			len;
	uint8_t 			audio_flags;
	uint8_t 			control_flags;
	uint32_t 			wakeup_id;
	float				master_volume;
	int16_t 			*adc_buffer;
	int16_t 			*dac_buffer;
}I2S_Drv_TypeDef;

/* status */
#define		I2S_STATUS_HALF			0x01
#define		I2S_STATUS_FULL			0x02
#define		I2S_STATUS_STEREO		0x20
#define		I2S_STATUS_DATA_READY	0x40
#define		I2S_STATUS_RUNNING		0x80
/* flags */
#define		I2S_FLAGS_WAKEUP			0x80
#define		I2S_FLAGS_USE_SYNTHMODULE	0x02
#define		I2S_FLAGS_USE_AUDIOMODULE	0x01


extern 	I2S_HandleTypeDef hi2s2;
extern	uint32_t	i2s_register(I2S_Drv_TypeDef *private_data);
extern	int16_t	*get_codec_out_buf(uint8_t handle);
extern	int16_t	*get_codec_in_buf(uint8_t handle);

#endif /* DRIVERS_ANALOG_INT_I2S_DRIVER_INT_I2S_DRIVER_H_ */
