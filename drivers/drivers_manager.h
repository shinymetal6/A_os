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
 * drivers_manager.h
 *
 *  Created on: Oct 10, 2024
 *      Author: fil
 */

#ifndef DRIVERS_DRIVERS_MANAGER_H_
#define DRIVERS_DRIVERS_MANAGER_H_

#include "../kernel/A.h"

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	char		driver_name[32];
	uint32_t	(*init)(void);
	uint32_t	(*deinit)(void);
	uint32_t	(*start)(void);
	uint32_t	(*stop)(void);
	uint32_t	(*get_status)(void);
	uint32_t	(*get_values)(uint8_t *values);
	uint32_t	(*set_status)(void);
	uint32_t	(*set_values)(uint8_t *values,uint8_t values_number);
	void 		(*periodic_before_check_timers_callback)(void);
	void 		(*periodic_after_check_timers_callback)(void);
}DriversDefsSensors_t;
/* status */
#define	DRIVER_IN_USE		0x80
#define	DRIVER_INITIALIZED	0x40
#define	DRIVER_FAILED		0x02
#define	DRIVER_REQUESTED	0x01
#define	DRIVER_UNUSED		0

#define	DRIVER_REQUEST_FAILED	0xffffffff
extern	DriversDefsSensors_t	DriversDefsSensors[MAX_SENSORS_DRIVERS];

extern	uint32_t	sensors_driver_register(DriversDefsSensors_t *driver);
extern	uint32_t	sensors_driver_unregister(DriversDefsSensors_t *driver);
extern	uint32_t 	sensors_driver_get_values(uint32_t handle,uint8_t *values);
extern	uint32_t	sensors_driver_init(void);
extern	uint32_t	sensors_driver_deinit(uint32_t handle);
extern	uint32_t	sensors_driver_start(uint32_t handle);
extern	uint32_t	sensors_driver_get_status(uint32_t handle);


#endif /* DRIVERS_DRIVERS_MANAGER_H_ */
