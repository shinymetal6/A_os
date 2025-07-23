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
 * sample_process_1_adc.c
 *
 *  Created on: Dec 5, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_ADC
extern	ADC_HandleTypeDef hadc1;
extern	TIM_HandleTypeDef htim6;

uint16_t	adc_data = 0;
ADC_Drv_TypeDef	ADC_Drv =
{
		.adc = &hadc1,
		.adc_buffer = &adc_data,
		.num_channels = 1,
		.adc_timer = &htim6,
		.flags = ADC_FLAGS_FULL_WAKEUP | ADC_FLAGS_CALIBRATE,
};
uint32_t		adc_driver_handle;
uint32_t		adc_ops=0;

void sample_process_1_adc(uint32_t process_id)
{
uint32_t	wakeup,flags;

	adc_driver_handle = int_adc_register(&ADC_Drv);
	adc_start(adc_driver_handle);

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);

	while(1)
	{
		wait_event(EVENT_TIMER|EVENT_USB_DEVICE_IRQ|EVENT_UART2_IRQ|EVENT_ADC2_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
		}
		if (( wakeup & WAKEUP_FROM_ADC1_IRQ) == WAKEUP_FROM_ADC1_IRQ)
		{
			adc_ops++;
		}

	}
}
#endif // #ifdef SAMPLEPROCESS_1_ADC
#endif // #ifdef SAMPLE_PROCESSES_ENABLED


