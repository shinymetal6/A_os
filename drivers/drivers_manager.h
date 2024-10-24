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
 *  Created on: Oct 15, 2024
 *      Author: fil
 */

#ifndef DRIVERS_DRIVERS_MANAGER_H_
#define DRIVERS_DRIVERS_MANAGER_H_

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	(*init)(uint8_t handle);
	uint32_t	(*deinit)(uint8_t handle);
	uint32_t	(*start)(uint8_t handle);
	uint32_t	(*stop)(uint8_t handle);
	uint32_t	(*get_status)(uint8_t handle);
	uint32_t	(*get_values)(uint8_t handle, uint8_t *values,uint8_t values_number);
	uint32_t	(*set_status)(uint8_t handle);
	uint32_t	(*set_values)(uint8_t handle, uint8_t *values,uint8_t values_number);
	uint32_t	(*extended_action)(uint32_t handle,uint32_t *action);
	void 		(*periodic_before_check_timers_callback)(void);
	void 		(*periodic_after_check_timers_callback)(void);
	uint32_t	*driver_private_data;
	char		driver_name[32];
}DriverStruct_t;

/* status */
#define	DRIVER_STATUS_IN_USE		0x80
#define	DRIVER_STATUS_INITIALIZED	0x40
#define	DRIVER_STATUS_INITPEND		0x20
#define	DRIVER_STATUS_REQUESTED		0x10
#define	DRIVER_STATUS_FAILED		0x01
#define	DRIVER_STATUS_UNUSED		0
/* flags */
#define	DRIVER_FLAGS_AUTOSTART		0x80

#define	DRIVER_REQUEST_FAILED	0xffffffff

extern	uint32_t 	driver_init(void);
extern	uint32_t	driver_register(DriverStruct_t *driver,uint32_t *private_drv_struct,uint32_t flags);
extern	uint32_t	driver_unregister(DriverStruct_t *driver);
extern	uint32_t 	driver_start(uint32_t handle);
extern	uint32_t 	driver_extended_action(uint32_t handle,uint32_t *action);
extern	uint32_t 	driver_get_values(uint32_t handle,uint8_t *values,uint8_t values_number);
extern	uint32_t 	driver_set_values(uint32_t handle,uint8_t *values,uint8_t values_number);
extern	uint32_t 	driver_scan(void);

#ifdef DHTXX_AM230X_ENABLE
#include "sensors/dhtxx_am230x/dhtxx_am230x.h"
#endif

#ifdef A_HAS_MOTOR_CNTRL
#include "actuators/pwm_control/pwm_control.h"
#endif

#ifdef DCC_SYSTEM_ENABLE
#include "actuators/dcc/dcc.h"
#endif

#ifdef INTERNAL_ADC_DRIVER
#include "analog/intadc_driver/intadc_driver.h"
#endif

#ifdef INTERNAL_DAC_DRIVER
#include "analog/intdac_driver/intdac_driver.h"
#endif


#endif /* DRIVERS_DRIVERS_MANAGER_H_ */
