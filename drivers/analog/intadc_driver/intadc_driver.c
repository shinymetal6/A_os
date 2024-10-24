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
 * intadc_driver.c
 *
 *  Created on: Oct 23, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#ifdef INTERNAL_ADC_DRIVER
#include "intadc_driver.h"
#include <string.h>

extern	DriverStruct_t			*DriverStruct[MAX_DRIVERS];

static uint32_t intadc_start(uint8_t handle)
{
ADC_Drv_TypeDef		*Adc_drv = (ADC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
TIM_HandleTypeDef	*timer = Adc_drv->adc_timer;
	HAL_TIM_Base_Start(timer);
	Adc_drv->status |= ADC_STATUS_RUNNING;
	return 0;
}

static uint32_t intadc_stop(uint8_t handle)
{
ADC_Drv_TypeDef		*Adc_drv = (ADC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
TIM_HandleTypeDef	*timer = Adc_drv->adc_timer;
	HAL_TIM_Base_Stop(timer);
	Adc_drv->status &= ~ADC_STATUS_RUNNING;
	return 0;
}

static uint32_t intadc_get_status(uint8_t handle)
{
	ADC_Drv_TypeDef		*Adc_drv = (ADC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
	return (uint32_t )Adc_drv->status;
}

static uint32_t intadc_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t intadc_set_values(uint8_t handle,uint8_t *values,uint8_t values_number)
{
	return 0;
}

static uint32_t intadc_extended_actions(uint32_t handle,uint32_t *action)
{
	return 0;
}

extern	DriverStruct_t	IntADC_Drv;

uint32_t intadc_deinit(uint8_t handle)
{
	return driver_unregister(&IntADC_Drv);
}

//uint32_t	pData[8];

static uint32_t intadc_init(uint8_t handle)
{
ADC_Drv_TypeDef		*Adc_drv = (ADC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
ADC_HandleTypeDef	*adc = Adc_drv->adc;
	Adc_drv->status &= ~(ADC_STATUS_HALF | ADC_STATUS_FULL);
	HAL_ADC_Start_DMA(adc, (uint32_t *)Adc_drv->adc_buffer, Adc_drv->num_channels);
	return 0;
}

DriverStruct_t	IntADC_Drv =
{
	.init = intadc_init,
	.deinit = intadc_deinit,
	.start = intadc_start,
	.stop = intadc_stop,
	.extended_action = intadc_extended_actions,
	.get_status = intadc_get_status,
	.get_values = intadc_get_values,
	.set_values = intadc_set_values,
	.periodic_before_check_timers_callback = NULL,
	.periodic_after_check_timers_callback = NULL,
	.driver_name = "Internal ADC",
};

uint32_t intadc_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&IntADC_Drv,sizeof(IntADC_Drv));
	return 0;
}

static uint32_t get_handle_from_adc_dma_channel(ADC_HandleTypeDef *hadc)
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
					ADC_Drv_TypeDef		*Adc_drv = (ADC_Drv_TypeDef	*)DriverStruct[i]->driver_private_data;
					if ( Adc_drv->adc == hadc )
						return i;
				}
			}
		}
	}
	return drv_ret;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
uint32_t handle;
	if ( (handle = get_handle_from_adc_dma_channel(hadc)) != 255 )
	{
		ADC_Drv_TypeDef		*Adc_drv = (ADC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
		Adc_drv->status |= ADC_STATUS_HALF;
		Adc_drv->status &= ~ADC_STATUS_FULL;
		if ( Adc_drv->flags & (ADC_FLAGS_HALF_WAKEUP | ADC_FLAGS_ALL_WAKEUP))
			activate_process(DriverStruct[handle]->process,EVENT_ADC1_IRQ,HW_ADC1);
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
uint32_t handle;
	if ( (handle = get_handle_from_adc_dma_channel(hadc)) != 255 )
	{
		ADC_Drv_TypeDef		*Adc_drv = (ADC_Drv_TypeDef	*)DriverStruct[handle]->driver_private_data;
		Adc_drv->status |= ADC_STATUS_FULL;
		Adc_drv->status &= ~ADC_STATUS_HALF;
		if ( Adc_drv->flags & (ADC_FLAGS_FULL_WAKEUP | ADC_FLAGS_ALL_WAKEUP))
			activate_process(DriverStruct[handle]->process,EVENT_ADC1_IRQ,HW_ADC1);
	}
}

#endif //#ifdef INTERNAL_ADC_DRIVER
