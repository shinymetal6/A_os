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
 * analog.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_ANALOG_H_
#define DRIVERS_ANALOG_ANALOG_H_

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	*analog_driver_private_data;
}ANALOG_DriverStruct_t;

#include "intadc_driver/intadc_driver.h"
#include "intdac_driver/intdac_driver.h"

#endif /* DRIVERS_ANALOG_ANALOG_H_ */
