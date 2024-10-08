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
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#ifdef DHT11_ENABLE
#include "dht11_drv.h"

Dht11_Drv_TypeDef	Dht11_Drv;

uint8_t dht11_start(void)
{
uint32_t	i;
	if (( Dht11_Drv.status & DHT11_RUNNING) == DHT11_RUNNING )
		return 1;
	for(i=0;i<DHT11_MAX_SAMPLES_LEN  ;i++)
		Dht11_Drv.dht11_samples[i] = 0;
	Dht11_Drv.status |= DHT11_RUNNING;
	return 0;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	Dht11_Drv.samples_number ++;
}

uint8_t dht11_get_status(void)
{
	return Dht11_Drv.status;
}

void dht11_create_bitbytes(void)
{
uint32_t	i,initial,temp;
	initial =  Dht11_Drv.dht11_samples[0];
	for(i=0;i<DHT11_MAX_SAMPLES_LEN;i++)
	{
		temp = Dht11_Drv.dht11_samples[i+1];
		Dht11_Drv.dht11_samples[i+1] -= initial;
		initial = temp;
	}
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
			GPIO_SetGpioOUT(GPIOPORT_DHT11,GPIOBIT_DHT11,0);
			Dht11_Drv.status |= DHT11_STARTBIT;
		}
		break;
	case	DHT11_START_BIT_SET:
		if ( Dht11_Drv.ticks == 0 )
		{
			Dht11_Drv.status &= ~DHT11_STARTBIT;
			Dht11_Drv.status |= DHT11_ACQRUN;
			GPIO_SetGpioAlternate(GPIOPORT_DHT11,GPIOBIT_DHT11);
			Dht11_Drv.samples_number = 0;
			HAL_TIM_IC_Start_DMA(&DHT11_TIMER,TIM_CHANNEL_4, Dht11_Drv.dht11_samples, DHT11_MAX_SAMPLES_LEN);
			Dht11_Drv.state_machine = DHT11_WAIT_FOR_TIM_END;
		}
		break;
	case	DHT11_WAIT_FOR_TIM_END:
		if ( Dht11_Drv.samples_number )
		{
			HAL_TIM_IC_Stop_DMA(&DHT11_TIMER,TIM_CHANNEL_4);
			GPIO_SetGpioIN(GPIOPORT_DHT11,GPIOBIT_DHT11,1);
			Dht11_Drv.state_machine = DHT11_END;
		}
		break;
	case	DHT11_END:
		Dht11_Drv.status &= ~(DHT11_RUNNING | DHT11_ACQRUN);
		Dht11_Drv.state_machine = DHT11_IDLE;
		dht11_create_bitbytes();
		break;
	}
}

void dht11_init(void)
{
	Dht11_Drv.state_machine = DHT11_IDLE;
	Dht11_Drv.ticks = 0;
	set_after_check_timers_callback(dht11_worker);
}

#endif



