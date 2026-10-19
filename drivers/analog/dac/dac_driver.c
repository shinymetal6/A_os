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
 * dac_driver.c
 *
 *  Created on: Oct 29, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_DAC_ENABLED

#include "dac_driver.h"
uint32_t	*first_dac = NULL;

ITCM_AREA_CODE  uint32_t dac_start(DAC_DriverStruct_t *dac_drv)
{
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	HAL_TIM_Base_Start_IT(timer);
	dac_drv->status |= DAC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE  uint32_t dac_stop(DAC_DriverStruct_t *dac_drv)
{
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	HAL_TIM_Base_Stop_IT(timer);
	dac_drv->status &= ~DAC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE  uint32_t dac_init(DAC_DriverStruct_t *dac_drv)
{
	dac_drv->status = HAL_DAC_Start_DMA(dac_drv->dac, dac_drv->channel, (uint32_t *)dac_drv->dac_buffer, dac_drv->dac_buf_size,dac_drv->alignment);
	return dac_drv->status;
}

ITCM_AREA_CODE  uint32_t dac_get_status(DAC_DriverStruct_t *dac_drv)
{
	return dac_drv->status;
	return 0;
}

ITCM_AREA_CODE   uint32_t dac_timer_set(DAC_DriverStruct_t *dac_drv,float frequency)
{
TIM_HandleTypeDef 	*dac_timer = dac_drv->dac_timer;
uint32_t 			timer_clk = HAL_RCC_GetPCLK1Freq() * 2;

	dac_drv->ARR = (timer_clk / dac_drv->sample_rate) - 1;
	dac_drv->PSC = 0;

    __HAL_TIM_SET_PRESCALER(dac_timer,dac_drv->PSC);
    __HAL_TIM_SET_AUTORELOAD(dac_timer, dac_drv->ARR);
    __HAL_TIM_SET_COUNTER(dac_timer, 0);
	return 0;
}

ITCM_AREA_CODE uint32_t	dac_register(DAC_DriverStruct_t *dac)
{
	if (dac == NULL)
		return 1;
	if (dac->dac_buffer == NULL)
		return 1;
	if ( (dac->dac == NULL) || (dac->dac_timer == NULL))
		return 1;

	if ( first_dac == NULL )
	{
		dac->pre_drv = (uint32_t *)&first_dac;
		dac->next_drv = NULL;
		first_dac = (uint32_t *)dac;
	}
	else
	{
		DAC_DriverStruct_t	*current_dac = (DAC_DriverStruct_t	*)first_dac;
		while(current_dac->next_drv != NULL)
			current_dac = (DAC_DriverStruct_t *)current_dac->next_drv;
		current_dac->next_drv = (uint32_t *)dac;
		dac->pre_drv = (uint32_t *)current_dac;
		dac->next_drv = NULL;
	}
	dac->process = get_current_process();
	dac_timer_set(dac,dac->sample_rate);

	return 0;
}

/************ Interrupt *************/

void dac_irq_common(DAC_HandleTypeDef *hdac,uint8_t full)
{
DAC_DriverStruct_t *dac_drv = (DAC_DriverStruct_t *)first_dac;
	if ( dac_drv != NULL )
	{
		while((dac_drv != NULL) && (dac_drv->dac != hdac))
			dac_drv = (DAC_DriverStruct_t *)dac_drv->next_drv;
		if (dac_drv != NULL)
		{
			if ( full )
				dac_drv->status |= DAC_STATUS_FULL;
			else
				dac_drv->status &= ~DAC_STATUS_FULL;
			if ( dac_drv->flags & DAC_FLAGS_WAKEUP)
				activate_process(dac_drv->process,EVENT_DAC_IRQ,HW_DAC);
		}
	}
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
	dac_irq_common(hdac,0);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
	dac_irq_common(hdac,1);
}

#endif // #ifdef A_OS_DAC_ENABLED


