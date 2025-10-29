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
 * adc_driver.c
 *
 *  Created on: Oct 29, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_ADC_ENABLED

#include "adc_driver.h"
#include <string.h>

uint32_t	*first_adc = NULL;

ITCM_AREA_CODE uint32_t adc_start(ADC_DriverStruct_t *adc_drv)
{
TIM_HandleTypeDef	*timer = adc_drv->adc_timer;
	adc_drv->status &= ~(ADC_STATUS_HALF | ADC_STATUS_FULL);
	if (( adc_drv->flags & ADC_FLAGS_CALIBRATE) == ADC_FLAGS_CALIBRATE)
	{
#ifdef A_OS_ADC_CALIBRATION_3PARAMS
		/* defaults to Channel in mode linear calibration offset */
		if (( adc_drv->flags & ADC_FLAGS_CALIBRATION_LINEARITY) == ADC_FLAGS_CALIBRATION_LINEARITY)
			HAL_ADCEx_Calibration_Start(adc_drv->adc, ADC_CALIB_OFFSET_LINEARITY,ADC_SINGLE_ENDED);
		else
			HAL_ADCEx_Calibration_Start(adc_drv->adc, ADC_CALIB_OFFSET,ADC_SINGLE_ENDED);
#else
		HAL_ADCEx_Calibration_Start(adc_drv->adc, ADC_SINGLE_ENDED);
#endif
		adc_drv->calibration = HAL_ADCEx_Calibration_GetValue(adc_drv->adc, ADC_SINGLE_ENDED);
	}
	else
		adc_drv->calibration = 0;

	if ( HAL_ADC_Start_DMA(adc_drv->adc, (uint32_t *)adc_drv->adc_buffer, adc_drv->num_channels)  == 0 )
	{
		HAL_TIM_Base_Start(timer);
		adc_drv->status |= ADC_STATUS_RUNNING;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE uint32_t adc_stop(ADC_DriverStruct_t *adc_drv)
{
TIM_HandleTypeDef	*timer = adc_drv->adc_timer;
	HAL_TIM_Base_Stop(timer);
	adc_drv->status &= ~ADC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE  uint32_t adc_get_status(ADC_DriverStruct_t *adc_drv)
{
	return (uint32_t )adc_drv->status;
}

ITCM_AREA_CODE  uint32_t adc_init(ADC_DriverStruct_t *adc_drv)
{
	adc_drv->status &= ~(ADC_STATUS_HALF | ADC_STATUS_FULL);
	return 0;
}

ITCM_AREA_CODE uint32_t	adc_register(ADC_DriverStruct_t *adc)
{
	if (adc == NULL)
		return 1;
	if (adc->adc_buffer == NULL)
		return 1;
	if ( (adc->adc == NULL) || (adc->adc_timer == NULL))
		return 1;

	if ( first_adc == NULL )
	{
		adc->pre_drv = (uint32_t *)&first_adc;
		adc->next_drv = NULL;
		first_adc = (uint32_t *)adc;
	}
	else
	{
		ADC_DriverStruct_t	*current_adc = (ADC_DriverStruct_t	*)first_adc;
		while(current_adc->next_drv != NULL)
			current_adc = (ADC_DriverStruct_t *)current_adc->next_drv;
		current_adc->next_drv = (uint32_t *)adc;
		adc->pre_drv = (uint32_t *)current_adc;
		adc->next_drv = NULL;
	}
	adc->process = get_current_process();
	return 0;
}

#endif // #ifdef A_OS_ADC_ENABLED


