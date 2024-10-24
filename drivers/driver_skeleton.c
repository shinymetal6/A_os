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
 *  Created on: Oct 15, 2024
 *      Author: fil
 */


#include "main.h"
#include "../kernel/system_default.h"
#include "../kernel/A.h"
#include "../kernel/A_exported_functions.h"
#include "../kernel/scheduler.h"

#include "driver_skeleton.h"
#include <string.h>

static uint32_t driver_skeleton_start(uint8_t handle)
{
	return 0;
}

static uint32_t driver_skeleton_stop(uint8_t handle)
{
	return 0;
}

static uint32_t driver_skeleton_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t driver_skeleton_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t driver_skeleton_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t driver_skeleton_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	DriverStruct_t	Skeleton_Drv;

uint32_t driver_skeleton_deinit(uint8_t handle)
{
	return driver_unregister(&Skeleton_Drv);
}

static uint32_t driver_skeleton_init(uint8_t handle)
{
	return 0;
}

DriverStruct_t	Skeleton_Drv =
{
	.init = driver_skeleton_init,
	.deinit = driver_skeleton_deinit,
	.start = driver_skeleton_start,
	.stop = driver_skeleton_stop,
	.extended_action = driver_skeleton_extended_actions,
	.get_status = driver_skeleton_get_status,
	.get_values = driver_skeleton_get_values,
	.set_values = driver_skeleton_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "driver_skeleton",
};

uint32_t driver_skeleton_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&Skeleton_Drv,sizeof(Skeleton_Drv));
	return 0;
}

