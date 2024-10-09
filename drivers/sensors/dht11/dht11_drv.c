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
	Dht11_Drv.status = DHT11_RUNNING;
	Dht11_Drv.state_machine = DHT11_IDLE;
	return 0;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	Dht11_Drv.status = DHT11_ACQDONE;
}

uint8_t dht11_get_status(void)
{
	return Dht11_Drv.status;
}

static void dht11_create_bitbytes(void)
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

uint8_t dht11_decode(void)
{
uint32_t	i,j,initial,byteindex;
uint8_t		byte_val,byte_mask;

	dht11_create_bitbytes();

	for(i=0;i<DHT11_MAX_SAMPLES_LEN;i++)
	{
		if (( Dht11_Drv.dht11_samples[i] > DHT11_START_MINIMUM) && ( Dht11_Drv.dht11_samples[i] < DHT11_START_MAXIMUM))
		{
			if (( Dht11_Drv.dht11_samples[i+1] > DHT11_START_MINIMUM) && ( Dht11_Drv.dht11_samples[i+1] < DHT11_START_MAXIMUM))
			{
				initial = i+2;
				break;
			}
		}
	}
	if ( i >= DHT11_MAX_SAMPLES_LEN-1)
		return 1;

	byteindex = 0;
	byte_val = 0;
	byte_mask = 0x80;
	Dht11_Drv.dht11_decoded[byteindex] = 0;
	for(i=initial;i<DHT11_MAX_SAMPLES_LEN;i+=2)
	{
		if (( Dht11_Drv.dht11_samples[i] >= DHT11_50uLOW_MIN ) && ( Dht11_Drv.dht11_samples[i] <= DHT11_50uLOW_MAX ))
		{
			if ( Dht11_Drv.dht11_samples[i+1] >= DHT11_DECODED_1MIN )	//	decoded 1
				byte_val |= byte_mask;
			byte_mask >>= 1;
			if ( byte_mask == 0 )
			{
				Dht11_Drv.dht11_decoded[byteindex] = byte_val;
				Dht11_Drv.checksum += byte_val;
				byteindex ++;
				if ( byteindex == DHT11_BYTES_NUM)
				{
					Dht11_Drv.checksum = 0;
					for(j=0;j<DHT11_BYTES_NUM-1;j++)
						Dht11_Drv.checksum += Dht11_Drv.dht11_decoded[j];
					if ( Dht11_Drv.checksum == Dht11_Drv.dht11_decoded[DHT11_BYTES_NUM-1])
						return 0;
					return 1;
				}
				byte_val = 0;
				byte_mask = 0x80;
			}
		}
	}
	return 1;
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
			Dht11_Drv.ticks = DHT11_CYCLE_TICKS;
		}
		break;
	case	DHT11_WAIT_FOR_TIM_END:
		if ( Dht11_Drv.ticks == 0 )
		{
			HAL_TIM_IC_Stop_DMA(&DHT11_TIMER,TIM_CHANNEL_4);
			GPIO_SetGpioIN(GPIOPORT_DHT11,GPIOBIT_DHT11,1);
			Dht11_Drv.state_machine = DHT11_END;
		}
		break;
	case	DHT11_END:
		Dht11_Drv.status = DHT11_ACQDONE;
		Dht11_Drv.state_machine = DHT11_IDLE;
		if ( dht11_decode() == 0 )
			Dht11_Drv.status |= DHT11_VALID;
		break;
	}
}

uint8_t dht11_get_values(uint8_t *values)
{
uint8_t j;
	if (( Dht11_Drv.status & DHT11_VALID) == DHT11_VALID)
	{
		for(j=0;j<DHT11_BYTES_NUM-1;j++)
			values[j] = Dht11_Drv.dht11_decoded[j];
	}
	else
		return 1;
	return 0;
}

void dht11_init(void)
{
	Dht11_Drv.state_machine = DHT11_IDLE;
	Dht11_Drv.ticks = 0;
	set_after_check_timers_callback(dht11_worker);
}

#endif



