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
 * driver_skeleton.c
 *
 *  Created on: Oct 14, 2024
 *      Author: fil
 */


#include "main.h"
/* Set the following includes with appropriate path */
#include "../kernel/system_default.h"
#include "../kernel/A.h"
#include "../kernel/A_exported_functions.h"
#include "../kernel/scheduler.h"

/* Uncomment and set the following ifdef to enable / disable from config file */
//#ifdef A_HAS_SKEL_modifyme_DEFINED

#include "driver_skeleton.h"

driver_skel_Drv_TypeDef	driver_skel_Drv;

static uint32_t driver_skel_init(void)
{
	return 0;
}

static uint32_t driver_skel_start(void)
{
	return 0;
}

static uint32_t driver_skel_stop(void)
{
	return 0;
}

static uint32_t driver_skel_get_status(void)
{
	return 0;
}

static uint32_t driver_skel_get_values(uint8_t *values)
{
	return 0;
}

static uint32_t driver_skel_select_channel(uint8_t channel)
{
	return 0;
}

static uint32_t driver_skel_set_values(uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t driver_skel_extended_actions(uint8_t action,uint32_t action_parameter,uint32_t extension_parameter)
{
	return 0;
}

extern	DriversDefsSensors_t	driver_skel_driver;

uint32_t driver_skel_deinit(void)
{
	return sensors_driver_unregister(&driver_skel_driver);
}

DriversDefsSensors_t	driver_skel_driver =
{
		.periodic_before_check_timers_callback = NULL,
		.periodic_after_check_timers_callback = NULL,
		.peripheral = NULL,
		.peripheral_channel = 0,
		.init = driver_skel_init,
		.deinit = driver_skel_deinit,
		.start = driver_skel_start,
		.stop = driver_skel_stop,
		.select_channel = driver_skel_select_channel,
		.extended_action = driver_skel_extended_actions,
		.get_status = driver_skel_get_status,
		.get_values = driver_skel_get_values,
		.set_values = driver_skel_set_values,
		.driver_name = "driver_skel_driver",
};

DriversDefsSensors_t *driver_skel__set_drv_struct(void)
{
	return &driver_skel_driver;
}

/* Uncomment the following endif */
//#endif // #ifdef A_HAS_MOTOR_CNTRL

