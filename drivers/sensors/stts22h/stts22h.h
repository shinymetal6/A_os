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
 *  Created on: Oct 26, 2024
 *      Author: fil
 */

#ifndef DRIVERS_SENSORS_STTS22H_STTS22H_H_
#define DRIVERS_SENSORS_STTS22H_STTS22H_H_

#define STTS22H_ID				0xA0U
#define	STTS22H_ADDR			0x7e
#define	STTS22H_T_LEN			2

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
#define	STTS22H_CONTINUOUS_REG_CTRL	0x0c

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				opmode;
	uint8_t				*data;
	uint8_t				whoami;
	I2C_HandleTypeDef 	*bus;
	uint16_t 			device_address;
	uint16_t 			address;
	GPIO_TypeDef	 	*power_port;
	uint16_t			power_bit;
	uint8_t				power_active_level;
}Stts22h_Drv_TypeDef;
/* status */
#define	STTS22H_STARTED		0x80
#define	STTS22H_STOPPED		0x00

#define STTS22H_I2C_TIMEOUT			1000U

extern	uint32_t stts22h_allocate_driver(DriverStruct_t *new_struct);

#endif /* DRIVERS_SENSORS_STTS22H_STTS22H_H_ */
