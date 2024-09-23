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
 * internal_adc.h
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#ifndef DRIVERS_INTERNAL_ADC_INTERNAL_ADC_H_
#define DRIVERS_INTERNAL_ADC_INTERNAL_ADC_H_

#ifdef INTERNAL_ADC_ENABLED

#define HW_ADC_ERROR_NONE			0
#define HW_ADC_ERROR_HW_NOT_OWNED	1
#define HW_ADC_GENERIC_ERROR		2

extern	uint8_t IntAdc_Init(uint8_t hw_adc_index,uint32_t *analog_buffer,uint32_t len);

extern	uint8_t IntAdc_Start(uint8_t hw_adc_index);
extern	uint8_t IntAdc_Stop(uint8_t hw_adc_index);

#endif // #ifdef INTERNAL_ADC_ENABLED

#endif /* DRIVERS_INTERNAL_ADC_INTERNAL_ADC_H_ */
