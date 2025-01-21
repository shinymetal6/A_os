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
 * int_adc_driver.c
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"

#ifdef A_OS_ADC_ENABLED

#include "int_adc_driver.h"
#include <string.h>

extern	ANALOG_DriverStruct_t			ANALOG_DriverStruct[MAX_ANALOG_DRIVERS];
extern	uint8_t							last_analog_used_handle,analog_driver_request;

ITCM_AREA_CODE static uint32_t int_adc_start(uint8_t handle)
{
ADC_Drv_TypeDef		*adc_drv = (ADC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
TIM_HandleTypeDef	*timer = adc_drv->adc_timer;
	adc_drv->status &= ~(ADC_STATUS_HALF | ADC_STATUS_FULL);
	/*
	if ( HAL_ADC_Start_DMA(adc_drv->adc, (uint32_t *)adc_drv->adc_buffer, adc_drv->num_channels)  == 0 )
	{
		HAL_TIM_Base_Start(timer);
		adc_drv->status |= ADC_STATUS_RUNNING;
		return 0;
	}
	return 1;
	*/
	adc_drv->status |= ADC_STATUS_RUNNING;
	return HAL_TIM_Base_Start(timer);
}

ITCM_AREA_CODE static uint32_t int_adc_stop(uint8_t handle)
{
ADC_Drv_TypeDef		*adc_drv = (ADC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
TIM_HandleTypeDef	*timer = adc_drv->adc_timer;
	HAL_TIM_Base_Stop(timer);
	adc_drv->status &= ~ADC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE static uint32_t int_adc_get_status(uint8_t handle)
{
ADC_Drv_TypeDef		*adc_drv = (ADC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
	return (uint32_t )adc_drv->status;
}

ITCM_AREA_CODE static uint32_t int_adc_init(uint8_t handle)
{
ADC_Drv_TypeDef		*adc_drv = (ADC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
ADC_HandleTypeDef	*adc = adc_drv->adc;
	adc_drv->status &= ~(ADC_STATUS_HALF | ADC_STATUS_FULL);
	return HAL_ADC_Start_DMA(adc, (uint32_t *)adc_drv->adc_buffer, adc_drv->num_channels);
}

ITCM_AREA_CODE uint32_t	int_adc_register(ADC_Drv_TypeDef *private_data,uint32_t driver_flags)
{
ADC_Drv_TypeDef	*adc_drv;
	if ( ANALOG_DriverStruct[last_analog_used_handle].process == 0 )
	{
		ANALOG_DriverStruct[last_analog_used_handle].process = get_current_process();
		ANALOG_DriverStruct[last_analog_used_handle].flags |= driver_flags;
		ANALOG_DriverStruct[last_analog_used_handle].private_data = (uint32_t *)private_data;

		adc_drv = (ADC_Drv_TypeDef *)ANALOG_DriverStruct[last_analog_used_handle].private_data;
		if ( adc_drv->adc == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( adc_drv->adc_timer == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( adc_drv->flags != 0 )
		{
			if ( adc_drv->wakeup_id == 0)
				return DRIVER_REQUEST_FAILED;
		}

		ANALOG_DriverStruct[last_analog_used_handle].status = DRIVER_STATUS_IN_USE;
		ANALOG_DriverStruct[last_analog_used_handle].adc_start = int_adc_start;
		ANALOG_DriverStruct[last_analog_used_handle].adc_stop = int_adc_stop;
		ANALOG_DriverStruct[last_analog_used_handle].adc_get_status = int_adc_get_status;
		ANALOG_DriverStruct[last_analog_used_handle].adc_init = int_adc_init;

		last_analog_used_handle++;
		analog_driver_request++;
		return last_analog_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}


/************ Interrupt *************/

ITCM_AREA_CODE static uint32_t get_handle_from_adc_dma_channel(ADC_HandleTypeDef *hadc)
{
uint32_t	i,drv_ret=255;
	for(i=0;i<MAX_ANALOG_DRIVERS;i++)
	{
		if (( ANALOG_DriverStruct[i].status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
		{
			if ( ANALOG_DriverStruct[i].private_data != NULL )
			{
				ADC_Drv_TypeDef	*adc_drv = (ADC_Drv_TypeDef	*)ANALOG_DriverStruct[i].private_data;
				if ( adc_drv->adc == hadc )
					return i;
			}
		}
	}
	return drv_ret;
}

ITCM_AREA_CODE void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
uint32_t handle;
	if ( (handle = get_handle_from_adc_dma_channel(hadc)) != 255 )
	{
		ADC_Drv_TypeDef	*adc_drv = (ADC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
		adc_drv->status |= ADC_STATUS_HALF;
		adc_drv->status &= ~ADC_STATUS_FULL;
		if ( adc_drv->flags & (ADC_FLAGS_HALF_WAKEUP | ADC_FLAGS_ALL_WAKEUP))
			activate_process(ANALOG_DriverStruct[handle].process,adc_drv->wakeup_id,adc_drv->wakeup_id);
	}
}

ITCM_AREA_CODE void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
uint32_t handle;
	if ( (handle = get_handle_from_adc_dma_channel(hadc)) != 255 )
	{
		ADC_Drv_TypeDef	*adc_drv = (ADC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
		adc_drv->status |= ADC_STATUS_FULL;
		adc_drv->status &= ~ADC_STATUS_HALF;
		if ( adc_drv->flags & (ADC_FLAGS_FULL_WAKEUP | ADC_FLAGS_ALL_WAKEUP))
			activate_process(ANALOG_DriverStruct[handle].process,adc_drv->wakeup_id,adc_drv->wakeup_id);
	}
}

#endif // #ifdef A_OS_ADC_ENABLED
