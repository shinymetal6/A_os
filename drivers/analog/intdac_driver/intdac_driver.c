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
 * intdac_driver.c
 *
 *  Created on: Oct 23, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#ifdef INTERNAL_DAC_DRIVER
#include "intdac_driver.h"
#include <string.h>

extern	DriverStruct_t			*DriverStruct[MAX_DRIVERS];
static uint32_t intdac_start(uint8_t handle)
{
DAC_Drv_TypeDef		*Dac_drv = (DAC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
TIM_HandleTypeDef	*timer = Dac_drv->dac_timer;
	HAL_TIM_Base_Start(timer);
	Dac_drv->status |= DAC_STATUS_RUNNING;
	return 0;
}

static uint32_t intdac_stop(uint8_t handle)
{
DAC_Drv_TypeDef		*Dac_drv = (DAC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
TIM_HandleTypeDef	*timer = Dac_drv->dac_timer;
	HAL_TIM_Base_Stop(timer);
	Dac_drv->status &= ~DAC_STATUS_RUNNING;
	return 0;
}

static uint32_t intdac_get_status(uint8_t handle)
{
	return 0;
}

static uint32_t intdac_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t intdac_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t intdac_extended_actions(uint8_t handle,uint8_t action,uint32_t action_parameter,uint32_t extension_parameter)
{
	return 0;
}

extern	DriverStruct_t	IntDAC_Drv;

uint32_t intdac_deinit(uint8_t handle)
{
	return driver_unregister(&IntDAC_Drv);
}

static uint32_t intdac_init(uint8_t handle)
{
	DAC_Drv_TypeDef		*Dac_drv = (DAC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	Dac_drv->status = 0;
	HAL_DAC_Start_DMA(Dac_drv->dac, Dac_drv->channel, (uint32_t *)Dac_drv->dac_buffer, Dac_drv->len,Dac_drv->alignment);
	return 0;
}

DriverStruct_t	IntDAC_Drv =
{
	.init = intdac_init,
	.deinit = intdac_deinit,
	.start = intdac_start,
	.stop = intdac_stop,
	.extended_action = intdac_extended_actions,
	.get_status = intdac_get_status,
	.get_values = intdac_get_values,
	.set_values = intdac_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "Internal DAC",
};

uint32_t intdac_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&IntDAC_Drv,sizeof(IntDAC_Drv));
	return 0;
}

static uint32_t get_handle_from_dac_dma_channel(DAC_HandleTypeDef *hdac)
{
uint32_t	i,drv_ret=255;
	for(i=0;i<MAX_DRIVERS;i++)
	{
		if ( DriverStruct[i] != NULL )
		{
			if (( DriverStruct[i]->status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
			{
				if ( DriverStruct[i]->driver_private_data != NULL)
				{
					DAC_Drv_TypeDef		*Dac_drv = (DAC_Drv_TypeDef	*)DriverStruct[i]->driver_private_data;
					if ( Dac_drv->dac == hdac )
						return i;
				}
			}
		}
	}
	return drv_ret;
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
uint32_t handle;
	if ( (handle = get_handle_from_dac_dma_channel(hdac)) != 255 )
	{
		DAC_Drv_TypeDef		*Dac_drv = (DAC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
		Dac_drv->status |= DAC_STATUS_HALF;
		Dac_drv->status &= ~DAC_STATUS_FULL;
		if ( Dac_drv->flags & (DAC_FLAGS_HALF_WAKEUP | DAC_FLAGS_ALL_WAKEUP))
			activate_process(DriverStruct[handle]->process,EVENT_DAC_IRQ,HW_DAC);
	}
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
uint32_t handle;
	if ( (handle = get_handle_from_dac_dma_channel(hdac)) != 255 )
	{
		DAC_Drv_TypeDef		*Dac_drv = (DAC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
		Dac_drv->status |= DAC_STATUS_FULL;
		Dac_drv->status &= ~DAC_STATUS_HALF;
		if ( Dac_drv->flags & (DAC_FLAGS_FULL_WAKEUP | DAC_FLAGS_ALL_WAKEUP))
			activate_process(DriverStruct[handle]->process,EVENT_DAC_IRQ,HW_DAC);
	}
}

#endif // #ifdef INTERNAL_DAC_DRIVER
