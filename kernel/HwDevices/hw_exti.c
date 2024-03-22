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
 * hw_exti.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */

#include "main.h"
#include "hw_spi.h"
#include "../A.h"
#include "../scheduler.h"
#include "../A_exported_functions.h"
#include "../hwmanager.h"
#include "../system_default.h"

extern	HWMngr_t		HWMngr[PERIPHERAL_NUM];
extern	HWDevices_t		HWDevices[HWDEVICES_NUM];

extern	PCB_t 		process[MAX_PROCESS];
extern	Asys_t		Asys;

#ifdef ENCODER_CLOCK_Pin
extern	void encoder_clock_irq_callback(void);
extern	void irq0_callback(void);
#endif
#if defined ENCODER_BTN_Pin
extern	void encoder_button_irq_callback(void);
extern	void irq1_callback(void);
#endif

#ifndef STM32H743xx
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{

}
#endif

#if defined SX126X_DIO1_PIN_NUMBER
extern	void RadioOnDioIrq( void );
#endif

uint32_t	flag_a = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#if defined NRF24L01_IRQ_PIN_PORT
	if(GPIO_Pin == NRF24L01_IRQ_PIN_NUMBER)
	{
		if ( HWDevices[HWDEV_NRF24L01].irq_callback != NULL )
			HWDevices[HWDEV_NRF24L01].irq_callback();
		if (( HWDevices[HWDEV_NRF24L01].status & HWDEV_STATUS_PRC_WAKEUP ) == HWDEV_STATUS_PRC_WAKEUP )
			activate_process(HWDevices[HWDEV_NRF24L01].process,1<<HWDevices[HWDEV_NRF24L01].bus,HWDEV_NRF24L01);
	}
#endif

#if defined SX126X_DIO1_PIN_NUMBER
	if(GPIO_Pin == SX126X_DIO1_PIN_NUMBER)
	{
		//RadioOnDioIrq();
	}
#endif
#ifdef ENCODER_BTN_Pin
	if(GPIO_Pin == ENCODER_BTN_Pin)
	{
		encoder_button_irq_callback();
	}
#endif
#ifdef ENCODER_CLOCK_Pin
	if(GPIO_Pin == ENCODER_CLOCK_Pin)
	{
		encoder_clock_irq_callback();
	}
#endif
}
