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
 * stts22h.h
 *
 *  Created on: Dec 23, 2024
 *      Author: fil
 */
#ifndef DRIVERS_I2C_SENSORS_STTS22H_STTS22H_H_
#define DRIVERS_I2C_SENSORS_STTS22H_STTS22H_H_

#define	STTS22H_I2C_TIMEOUT				1000

#define	STTS22H_ADDR					0x7e
#define	STTS22H_T_LEN					2
#define	STTS22H_STARTED					0x80


/* who am i */
#define STTS22H_ID				0xA0U
/* registers */
#define STTS22H_WHOAMI			0x01U
#define STTS22H_TEMP_H_LIMIT	0x02U
#define STTS22H_TEMP_L_LIMIT	0x03U
#define STTS22H_CTRL			0x04U
#define STTS22H_STATUS			0x05U
#define STTS22H_TEMP_L_OUT		0x06U
#define STTS22H_TEMP_H_OUT		0x07U
#define STTS22H_SOFTWARE_RESET	0x0CU

#define	STTS22H_ONE_SHOT_REG_CTRL	0x01

extern	uint32_t stts22h_register(I2C_Sensors_DriverStruct_t *driver_private_data);

#endif /* DRIVERS_I2C_SENSORS_STTS22H_STTS22H_H_ */
