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
 * dht11_drv.c
 *
 *  Created on: Oct 8, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#define	DHT11_ENABLE	1
#ifdef DHT11_ENABLE
#include "dht11_drv.h"

Dht11_Drv_TypeDef	Dht11_Drv;

void dht11_setgpio_gpioOUT(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
	GPIOx->MODER &= ~((1 << GPIO_Pin) | (1 << (GPIO_Pin+1)));
	GPIOx->MODER |= 1 << GPIO_Pin;
	if (PinState != GPIO_PIN_RESET)
		GPIOx->BSRR = GPIO_Pin;
	else
		GPIOx->BSRR = (uint32_t)GPIO_Pin << 16;
}

void dht11_setgpio_gpioIN(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint16_t pullup)
{
	GPIOx->MODER &= ~((1 << GPIO_Pin) | (1 << (GPIO_Pin+1)));
	if ( pullup )
		GPIOx->PUPDR |= 1 << GPIO_Pin;
	else
		GPIOx->PUPDR &= ~(1 << GPIO_Pin);
}

void dht11_setgpio_special(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIOx->MODER &= ~((1 << GPIO_Pin) | (1 << (GPIO_Pin+1)));
	GPIOx->MODER |= 1 << (GPIO_Pin+1);
}

void dht11_start(void)
{
	Dht11_Drv.state_machine = DHT11_START;
	Dht11_Drv.status |= DHT11_RUNNING;
}

void dht11_worker(void)
{
	if ( Dht11_Drv.ticks )
		Dht11_Drv.ticks--;
	switch(Dht11_Drv.state_machine)
	{
	case	DHT11_IDLE:
		if (( Dht11_Drv.status & DHT11_RUNNING) == DHT11_RUNNING)
		{
			Dht11_Drv.state_machine = DHT11_START_BIT_SET;
			Dht11_Drv.ticks = DHT11_START_TICKS;
			dht11_setgpio_gpioOUT(GPIOPORT_DHT11,GPIOBIT_DHT11,0);
			Dht11_Drv.status |= DHT11_STARTBIT;
		}
		break;
	case	DHT11_START_BIT_SET:
		if ( Dht11_Drv.ticks == 0 )
		{
			Dht11_Drv.status &= ~DHT11_STARTBIT;
			Dht11_Drv.status |= DHT11_ACQRUN;
			dht11_setgpio_special(GPIOPORT_DHT11,GPIOBIT_DHT11);
			Dht11_Drv.samples_number = 0;
			HAL_TIM_Base_Start_DMA(&DHT11_TIMER, (uint32_t *)Dht11_Drv.dht11_samples, DHT11_MAX_LEN);
			Dht11_Drv.state_machine = DHT11_WAIT_FOR_TIM_END;
		}
		break;
	case	DHT11_WAIT_FOR_TIM_END:
		Dht11_Drv.samples_number ++;
		if ( Dht11_Drv.samples_number > (DHT11_MAX_LEN-1))
		{
			HAL_TIM_Base_Stop_DMA(&DHT11_TIMER);
			Dht11_Drv.status &= ~(DHT11_RUNNING | DHT11_ACQRUN);
			Dht11_Drv.state_machine = DHT11_END;
		}
		break;
	case	DHT11_END:
		Dht11_Drv.state_machine = DHT11_IDLE;
		break;
	}
}

void dht11_init(void)
{
	Dht11_Drv.state_machine = DHT11_IDLE;
	set_after_check_timers_callback(dht11_worker);
}

#endif



