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
ControlAdcDef		ControlAdc;

#ifdef ADC_SINGLE_CHANNEL

uint8_t IntAdc_Init(uint8_t hw_adc_index)
{
#ifdef ADC_HAS_OPAMP
	HAL_OPAMP_Start(&OPAMP_HANDLE);
#endif
	if ( HWMngr[hw_adc_index].process != Asys.current_process )
		return HW_ADC_ERROR_HW_NOT_OWNED;
	if ( HAL_ADC_Start_DMA(&ADC_HANDLE,(uint32_t *)&ControlAdc.analog_in[ADC_SINGLE_CHANNEL_NUMBER],2) )
		return 1;
	ControlAdc.hw_adc_index = hw_adc_index;
	return HW_ADC_ERROR_NONE;
}

uint8_t IntAdc_Start(void)
{
	return HAL_TIM_Base_Start(&ADC_TIMER);
}

uint8_t IntAdc_Stop(void)
{
	return HAL_TIM_Base_Stop(&ADC_TIMER);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if ( hadc == &ADC_HANDLE)
	{
		if ( ControlAdc.hw_adc_index == HW_ADC1)
		{
			ControlAdc.adc_flag |= INT_ADC_ANALOG_IN1_DONE;
			activate_process(HWMngr[HW_ADC1].process,EVENT_ADC1_IRQ,ControlAdc.analog_in[ADC_SINGLE_CHANNEL_NUMBER]);
		}
		if ( ControlAdc.hw_adc_index == HW_ADC2)
		{
			ControlAdc.adc_flag |= INT_ADC_ANALOG_IN2_DONE;
			activate_process(HWMngr[HW_ADC2].process,EVENT_ADC2_IRQ,ControlAdc.analog_in[ADC_SINGLE_CHANNEL_NUMBER]);
		}
	}
}

#else

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if ( hadc == &CONTROL_ADC1)
		ControlAdc.adc_flag |= INT_ADC_POT_DONE;
	if ( hadc == &ANALOG_IN_ADC2)
		ControlAdc.adc_flag |= INT_ADC_ANALOG_IN2_DONE;

}

uint8_t IntAdc_Start(void)
{
	if ( HWMngr[HW_ADC1].process != Asys.current_process )
		return HW_ADC_ERROR_HW_NOT_OWNED;
	if ( HWMngr[HW_ADC2].process != Asys.current_process )
		return HW_ADC_ERROR_HW_NOT_OWNED;
	HAL_ADC_Start_DMA(&CONTROL_ADC1,   (uint32_t *)&ControlAdc.pot[0], 6);
	HAL_ADC_Start_DMA(&ANALOG_IN_ADC2, (uint32_t *)&ControlAdc.analog_in[0], 4);
	HAL_TIM_Base_Start_IT(&CONTROL_TIMER);
	return HW_ADC_ERROR_NONE;
}
#endif

#endif	//#ifdef ADC_DAC_ENABLED


