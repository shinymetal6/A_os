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
 * Project : bb1xx_743_00 
*/
/*
 * internal_adc.c
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/system_default.h"

#ifdef INTERNAL_ADC_ENABLED

#include "../../kernel/scheduler.h"
#include "../../kernel/hwmanager.h"
#include "../../kernel/A_exported_functions.h"
#include "internal_adc.h"

extern	HWMngr_t	HWMngr[PERIPHERAL_NUM];
extern	Asys_t		Asys;

uint8_t IntAdc_Init(uint8_t hw_adc_index,uint32_t *analog_buffer,uint32_t len)
{
#ifdef ADC_HAS_OPAMP
	HAL_OPAMP_Start(&OPAMP_HANDLE);
#endif
	if ( HWMngr[hw_adc_index].process != Asys.current_process )
		return HW_ADC_ERROR_HW_NOT_OWNED;
	if ( hw_adc_index == HW_ADC1)
	{
		if ( HAL_ADC_Start_DMA(&ADC_HANDLE1,analog_buffer,len) )
			return 1;
	}
	if ( hw_adc_index == HW_ADC2)
	{
		if ( HAL_ADC_Start_DMA(&ADC_HANDLE2,analog_buffer,len) )
			return 1;
	}
	return HW_ADC_ERROR_NONE;
}

uint8_t IntAdc_Start(uint8_t hw_adc_index)
{
	if ( hw_adc_index == HW_ADC1 )
		return HAL_TIM_Base_Start(&ADC1_TIMER);
	if ( hw_adc_index == HW_ADC2 )
		return HAL_TIM_Base_Start(&ADC2_TIMER);
	return HW_ADC_GENERIC_ERROR;
}

uint8_t IntAdc_Stop(uint8_t hw_adc_index)
{
	if ( hw_adc_index == HW_ADC1 )
		return HAL_TIM_Base_Stop(&ADC1_TIMER);
	if ( hw_adc_index == HW_ADC2 )
		return HAL_TIM_Base_Stop(&ADC2_TIMER);
	return HW_ADC_GENERIC_ERROR;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if ( hadc == &ADC_HANDLE1)
		activate_process(HWMngr[HW_ADC1].process,EVENT_ADC1_IRQ,HW_ADC1);
	if ( hadc == &ADC_HANDLE2)
		activate_process(HWMngr[HW_ADC2].process,EVENT_ADC2_IRQ,HW_ADC2);
}

#endif	//#ifdef ADC_DAC_ENABLED


