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
 * int_adc_driver.h
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_INT_ADC_DRIVER_INT_ADC_DRIVER_H_
#define DRIVERS_ANALOG_INT_ADC_DRIVER_INT_ADC_DRIVER_H_

#ifdef A_OS_ADC_ENABLED

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	ADC_HandleTypeDef 	*adc;
	TIM_HandleTypeDef 	*adc_timer;
	uint16_t 			*adc_buffer;
	uint16_t 			num_channels;
	uint32_t 			wakeup_id;
}ADC_Drv_TypeDef;

/* status */
#define		ADC_STATUS_HALF			0x01
#define		ADC_STATUS_FULL			0x02
#define		ADC_STATUS_DATA_READY	0x40
#define		ADC_STATUS_RUNNING		0x80
/* flags */
#define		ADC_FLAGS_HALF_WAKEUP	0x20
#define		ADC_FLAGS_FULL_WAKEUP	0x40
#define		ADC_FLAGS_ALL_WAKEUP	0x80

extern uint32_t	int_adc_register(ADC_Drv_TypeDef *analog_driver_private_data,uint32_t driver_flags);
#endif // #ifdef A_OS_ADC_ENABLED

#endif /* DRIVERS_ANALOG_INT_ADC_DRIVER_INT_ADC_DRIVER_H_ */
