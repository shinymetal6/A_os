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
 * int_dac_driver.c
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

#include "int_dac_driver.h"

extern	ANALOG_DriverStruct_t			ANALOG_DriverStruct[MAX_ANALOG_DRIVERS];
extern	uint8_t							last_analog_used_handle,analog_driver_request;

ITCM_AREA_CODE  static uint32_t int_dac_start(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	HAL_TIM_Base_Start(timer);
	dac_drv->status |= DAC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_dac_stop(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	HAL_TIM_Base_Stop(timer);
	dac_drv->status &= ~DAC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_dac_init(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
	dac_drv->status = 0;
	HAL_DAC_Start_DMA(dac_drv->dac, dac_drv->channel, (uint32_t *)dac_drv->dac_buffer, dac_drv->len,dac_drv->alignment);
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_dac_get_status(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
	return dac_drv->status;
	return 0;
}

ITCM_AREA_CODE uint32_t	int_dac_register(DAC_Drv_TypeDef *analog_driver_private_data,uint32_t driver_flags)
{
DAC_Drv_TypeDef	*dac_drv;
	if ( ANALOG_DriverStruct[last_analog_used_handle].process == 0 )
	{
		ANALOG_DriverStruct[last_analog_used_handle].process = get_current_process();
		ANALOG_DriverStruct[last_analog_used_handle].flags |= driver_flags;
		ANALOG_DriverStruct[last_analog_used_handle].analog_driver_private_data = (uint32_t *)analog_driver_private_data;

		dac_drv = (DAC_Drv_TypeDef *)ANALOG_DriverStruct[last_analog_used_handle].analog_driver_private_data;
		if ( dac_drv->dac == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( dac_drv->dac_timer == NULL)
			return DRIVER_REQUEST_FAILED;

		ANALOG_DriverStruct[last_analog_used_handle].status = DRIVER_STATUS_IN_USE;
		ANALOG_DriverStruct[last_analog_used_handle].dac_start = int_dac_start;
		ANALOG_DriverStruct[last_analog_used_handle].dac_stop = int_dac_stop;
		ANALOG_DriverStruct[last_analog_used_handle].dac_get_status = int_dac_get_status;
		ANALOG_DriverStruct[last_analog_used_handle].dac_init = int_dac_init;
		last_analog_used_handle++;
		analog_driver_request++;
		return last_analog_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

/************ Interrupt *************/

ITCM_AREA_CODE  static uint32_t get_handle_from_dac_dma_channel(DAC_HandleTypeDef *hdac)
{
uint32_t	i,drv_ret=255;
	for(i=0;i<MAX_ANALOG_DRIVERS;i++)
	{
		if (( ANALOG_DriverStruct[i].status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
		{
			if ( ANALOG_DriverStruct[i].analog_driver_private_data != NULL )
			{
				DAC_Drv_TypeDef	*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[i].analog_driver_private_data;
				if ( dac_drv->dac == hdac )
					return i;
			}
		}
	}
	return drv_ret;
}

ITCM_AREA_CODE  static void dac_irq_common(DAC_Drv_TypeDef	*dac_drv,uint32_t handle)
{
	if (( dac_drv->flags & DAC_FLAGS_USE_AUDIOMODULE) == DAC_FLAGS_USE_AUDIOMODULE)
	{
		RunOscillator32();
		effects_apply(dac_drv->status & DAC_STATUS_FULL,AUDIO_IS_MONO,dac_drv->dac_buffer);
	}
	if ( dac_drv->flags & DAC_FLAGS_WAKEUP)
		activate_process(ANALOG_DriverStruct[handle].process,EVENT_DAC_IRQ,HW_DAC);
}

ITCM_AREA_CODE void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
uint32_t handle;
	__disable_irq();
	if ( (handle = get_handle_from_dac_dma_channel(hdac)) != 255 )
	{
		DAC_Drv_TypeDef	*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
		dac_drv->status |= DAC_STATUS_HALF;
		dac_drv->status &= ~DAC_STATUS_FULL;
		dac_irq_common(dac_drv,handle);
	}
	__enable_irq();
}

ITCM_AREA_CODE void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
uint32_t handle;
	__disable_irq();
	if ( (handle = get_handle_from_dac_dma_channel(hdac)) != 255 )
	{
		DAC_Drv_TypeDef	*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
		dac_drv->status |= DAC_STATUS_FULL;
		dac_drv->status &= ~DAC_STATUS_HALF;
		dac_irq_common(dac_drv,handle);
	}
	__enable_irq();
}


