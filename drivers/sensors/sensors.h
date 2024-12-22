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
 * sensors.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_SENSORS_SENSORS_H_
#define DRIVERS_SENSORS_SENSORS_H_

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	sensor_id;
	uint32_t	*private_data;
	uint32_t	(*sensor_start)  (uint8_t handle);
	uint32_t	(*sensor_stop)  (uint8_t handle);
	uint32_t	(*sensor_init) (uint8_t handle);
	void 		(*periodic_before_check_timers_callback)(void);
	void 		(*periodic_after_check_timers_callback)(void);

}Sensors_DriverStruct_t;

#include "dhtxx_am230x/dhtxx_am230x.h"

#endif /* DRIVERS_SENSORS_SENSORS_H_ */
