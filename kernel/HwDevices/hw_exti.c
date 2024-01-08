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

#if defined ST25R391X_INT_Pin
extern	void st25r3916Isr( void );
#endif

#ifdef BB1xx_743
extern	void irq0_callback(void);
extern	void irq1_callback(void);
#endif

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
#if defined ST25R391X_INT_Pin
	if ( GPIO_Pin == ST25R391X_INT_Pin)
		st25r3916Isr();
#endif
}

#if defined SX126X_DIO1_PIN_NUMBER
extern	void RadioOnDioIrq( void );
#endif

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#if defined EXTI4_NRFIRQ_Pin
	if(GPIO_Pin == EXTI4_NRFIRQ_Pin)
	{
		activate_process(1,WAKEUP_FROM_NRF24L01_IRQ,0);
	}
#endif
#if defined SX126X_DIO1_PIN_NUMBER
	if(GPIO_Pin == SX126X_DIO1_PIN_NUMBER)
	{
		RadioOnDioIrq();
	}
#endif
#ifdef BB1xx_743
#if defined ENCODER_BTN_Pin
	if(GPIO_Pin == ENCODER_BTN_Pin)
	{
		irq0_callback();
	}
	if(GPIO_Pin == ENCODER_CLOCK_Pin)
	{
		irq1_callback();
	}
#endif
#endif
}
