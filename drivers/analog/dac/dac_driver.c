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
	dac_drv->status = HAL_DAC_Start_DMA(dac_drv->dac, dac_drv->channel, (uint32_t *)dac_drv->dac_buffer, dac_drv->len,dac_drv->alignment);
	return dac_drv->status;
}

ITCM_AREA_CODE  uint32_t dac_get_status(DAC_DriverStruct_t *dac_drv)
{
	return dac_drv->status;
	return 0;
}

float			fdiv;
uint32_t		pkt_out=0;
extern uint32_t	usb_pkt;

ITCM_AREA_CODE   uint32_t dac_timer_set(DAC_DriverStruct_t *dac_drv,float frequency)
{
TIM_HandleTypeDef 	*dac_timer = dac_drv->dac_timer;

	dac_drv->dac_sample_frequency = frequency;
	//fdiv = ((float )HSI_CLOCK / 2.0F / frequency) - 1.0F;
	fdiv = ((float )HSI_CLOCK / 1.0F / frequency) - 1.0F;
	__HAL_TIM_DISABLE(dac_timer);
	dac_timer->Instance->CNT = 0;
	dac_drv->PSC = dac_timer->Instance->PSC = 0;
	dac_drv->ARR = dac_timer->Instance->ARR = (uint32_t )fdiv;
	__HAL_TIM_ENABLE(dac_timer);
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
	dac_timer_set(dac,dac->dac_sample_frequency);

	return 0;
}

#endif // #ifdef A_OS_DAC_ENABLED


