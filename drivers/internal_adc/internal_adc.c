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
#include "../../kernel/system_default.h"

#ifdef BB1xx_743
#include "internal_adc.h"
DMA_RAM		ControlAdcDef	ControlAdc;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if ( hadc == &CONTROL_ADC1)
		ControlAdc.adc_flag |= INT_ADC_POT_DONE;
	if ( hadc == &ANALOG_IN_ADC2)
		ControlAdc.adc_flag |= INT_ADC_ANALOG_IN_DONE;

}

void IntAdc_Start(void)
{
	HAL_ADC_Start_DMA(&CONTROL_ADC1,   (uint32_t *)&ControlAdc.pot[0], 6);
	HAL_ADC_Start_DMA(&ANALOG_IN_ADC2, (uint32_t *)&ControlAdc.analog_in[0], 4);
	HAL_TIM_Base_Start_IT(&CONTROL_TIMER);
}

#endif

