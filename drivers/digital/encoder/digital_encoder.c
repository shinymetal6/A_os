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
 * digital_encoder.c
 *
 *  Created on: Oct 23, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#include "digital_encoder.h"
#include <string.h>

extern	DriverStruct_t			*DriverStruct[MAX_DRIVERS];

static uint32_t digital_encoder_start(uint8_t handle)
{
	/*
DigitalEncoder_Drv_TypeDef	*DigEncoder_Drv = (DigitalEncoder_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
GPIO_TypeDef				*clock_port = DigEncoder_Drv->clock_port;
uint16_t					clock_bit;
GPIO_TypeDef				*data_port  = DigEncoder_Drv->data_port;
uint16_t					data_bit;
*/

	return 0;
}

static uint32_t digital_encoder_stop(uint8_t handle)
{
	return 0;
}

static uint32_t digital_encoder_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t digital_encoder_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t digital_encoder_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t digital_encoder_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	DriverStruct_t	DigitalEncoder_Drv;

uint32_t digital_encoder_deinit(uint8_t handle)
{
	return driver_unregister(&DigitalEncoder_Drv);
}

static uint32_t digital_encoder_init(uint8_t handle)
{
	return 0;
}

DriverStruct_t	DigitalEncoder_Drv =
{
	.init = digital_encoder_init,
	.deinit = digital_encoder_deinit,
	.start = digital_encoder_start,
	.stop = digital_encoder_stop,
	.extended_action = digital_encoder_extended_actions,
	.get_status = digital_encoder_get_status,
	.get_values = digital_encoder_get_values,
	.set_values = digital_encoder_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "digital_encoder",
};

uint32_t digital_encoder_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&DigitalEncoder_Drv,sizeof(DigitalEncoder_Drv));
	return 0;
}

