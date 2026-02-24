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
 * i2c.h
 *
 *  Created on: Oct 28, 2025
 *      Author: fil
 */

#ifndef DRIVERS_I2C_I2C_H_
#define DRIVERS_I2C_I2C_H_

#ifdef A_OS_I2C_ENABLED

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint32_t 			wakeup_id;
	uint32_t			*next_drv;
	/* driver proprietary data */
} I2C_DriverStruct_t;

extern	I2C_DriverStruct_t	*i2c_drv_ptr;

#ifdef SOUND_ENGINE_ENABLED
	#include "codecs/nau88c22.h"
#endif // #ifdef SOUND_ENABLED
#include "memories/i2c_24xx.h"
#include "sensors/accgyro/lis2dw12.h"
#include "sensors/humidity/sht40.h"
#include "sensors/temperature/stts22h.h"
#include "sensors/temperature/mlx90614.h"
#include "sensors/pressure/lps22df.h"
#include "sensors/others/mlx90640.h"

#endif // #ifdef A_OS_I2C_ENABLED

#endif /* DRIVERS_I2C_I2C_H_ */
