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
	uint32_t	*peripheral;
	uint32_t	peripheral_channel;
	uint32_t	(*init)(void);
	uint32_t	(*deinit)(void);
	uint32_t	(*start)(void);
	uint32_t	(*stop)(void);
	uint32_t	(*select_channel)(uint8_t controller_index);
	uint32_t	(*get_status)(void);
	uint32_t	(*get_values)(uint8_t *values);
	uint32_t	(*set_status)(void);
	uint32_t	(*set_values)(uint8_t *values,uint8_t values_number);
	uint32_t	(*extended_action)(uint8_t action,uint32_t action_parameter,uint32_t extension_parameter);
	void 		(*periodic_before_check_timers_callback)(void);
	void 		(*periodic_after_check_timers_callback)(void);
	char		driver_name[32];
}DriversDefsSensors_t;

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	*peripheral;
	uint32_t	peripheral_channel;
	uint32_t	(*init)(void);
	uint32_t	(*deinit)(void);
	uint32_t	(*start)(void);
	uint32_t	(*stop)(void);
	uint32_t	(*select_channel)(uint8_t controller_index);
	uint32_t	(*get_status)(void);
	uint32_t	(*get_values)(uint8_t *values);
	uint32_t	(*set_status)(void);
	uint32_t	(*set_values)(uint8_t *values,uint8_t values_number);
	uint32_t	(*extended_action)(uint8_t action,uint32_t action_parameter,uint32_t extension_parameter);
	void 		(*periodic_before_check_timers_callback)(void);
	void 		(*periodic_after_check_timers_callback)(void);
	char		driver_name[32];
}DriversDefsActuators_t;

/* status */
#define	DRIVER_STATUS_IN_USE		0x80
#define	DRIVER_STATUS_INITIALIZED	0x40
#define	DRIVER_STATUS_REQUESTED		0x20
#define	DRIVER_STATUS_FAILED		0x01
#define	DRIVER_STATUS_UNUSED		0
/* flags */
#define	DRIVER_FLAGS_AUTOSTART		0x80

#define	DRIVER_REQUEST_FAILED	0xffffffff
extern	DriversDefsSensors_t	DriversDefsSensors[MAX_SENSORS_DRIVERS];
extern	DriversDefsSensors_t	DriversDefsSensors[MAX_SENSORS_DRIVERS];

extern	uint32_t	sensors_driver_register(DriversDefsSensors_t *driver,uint32_t flags);
extern	uint32_t	sensors_driver_unregister(DriversDefsSensors_t *driver);
extern	uint32_t 	sensors_driver_get_values(uint32_t handle,uint8_t *values);
extern	uint32_t	sensors_driver_init(void);
extern	uint32_t	sensors_driver_deinit(uint32_t handle);
extern	uint32_t	sensors_driver_start(uint32_t handle);
extern	uint32_t	sensors_driver_get_status(uint32_t handle);
extern	uint32_t	sensors_driver_extended_action(uint32_t handle,uint8_t action,uint32_t action_parameter,uint32_t extension_parameter);

extern	uint32_t	actuators_driver_register(DriversDefsActuators_t *drive,uint32_t flagsr);
extern	uint32_t	actuators_driver_unregister(DriversDefsActuators_t *driver);
extern	uint32_t 	actuators_driver_get_values(uint32_t handle,uint8_t *values);
extern	uint32_t	actuators_driver_init(void);
extern	uint32_t	actuators_driver_deinit(uint32_t handle);
extern	uint32_t	actuators_driver_start(uint32_t handle);
extern	uint32_t	actuators_driver_get_status(uint32_t handle);
extern	uint32_t	actuators_driver_extended_action(uint32_t handle,uint8_t action,uint32_t action_parameter,uint32_t extension_parameter);

#ifdef DHTXX_AM230X_ENABLE
#include "sensors/dhtxx_am230x/dhtxx_am230x.h"
#endif

#ifdef A_HAS_MOTOR_CNTRL
#include "actuators/pwm_control/pwm_control.h"
#endif

#endif /* DRIVERS_DRIVERS_MANAGER_H_ */
