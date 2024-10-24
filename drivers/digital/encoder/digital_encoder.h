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
 * digital_encoder.h
 *
 *  Created on: Oct 23, 2024
 *      Author: fil
 */

#ifndef DRIVERS_DIGITAL_ENCODER_DIGITAL_ENCODER_H_
#define DRIVERS_DIGITAL_ENCODER_DIGITAL_ENCODER_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	GPIO_TypeDef	 	*clock_port;
	uint16_t			clock_bit;
	GPIO_TypeDef	 	*data_port;
	uint16_t			data_bit;
	char				driver_name[32];
}DigitalEncoder_Drv_TypeDef;

#endif /* DRIVERS_DIGITAL_ENCODER_DIGITAL_ENCODER_H_ */
