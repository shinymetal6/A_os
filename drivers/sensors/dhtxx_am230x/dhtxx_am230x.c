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
 * dhtxx_am230x.c
 *
 *  Created on: Oct 9, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"

#ifdef DHTXX_AM230X_ENABLE
#include "dhtxx_am230x.h"

Dhtxx_am230x_Drv_TypeDef	Dhtxx_am230x_Drv;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	Dhtxx_am230x_Drv.status = DHTXX_AM230X_ACQDONE;
}

static void dhtxx_am230x_create_bitbytes(void)
{
uint32_t	i,initial,temp;
	initial =  Dhtxx_am230x_Drv.dhtxx_am230x_samples[0];
	for(i=0;i<DHTXX_AM230X_MAX_SAMPLES_LEN;i++)
	{
		temp = Dhtxx_am230x_Drv.dhtxx_am230x_samples[i+1];
		Dhtxx_am230x_Drv.dhtxx_am230x_samples[i+1] -= initial;
		initial = temp;
	}
}

static uint8_t dhtxx_am230x_decode(void)
{
uint32_t	i,j,initial,byteindex;
uint8_t		byte_val,byte_mask;

	dhtxx_am230x_create_bitbytes();

	for(i=0;i<DHTXX_AM230X_MAX_SAMPLES_LEN;i++)
	{
		if (( Dhtxx_am230x_Drv.dhtxx_am230x_samples[i] > DHTXX_AM230X_START_MINIMUM) && ( Dhtxx_am230x_Drv.dhtxx_am230x_samples[i] < DHTXX_AM230X_START_MAXIMUM))
		{
			if (( Dhtxx_am230x_Drv.dhtxx_am230x_samples[i+1] > DHTXX_AM230X_START_MINIMUM) && ( Dhtxx_am230x_Drv.dhtxx_am230x_samples[i+1] < DHTXX_AM230X_START_MAXIMUM))
			{
				initial = i+2;
				break;
			}
		}
	}
	if ( i >= DHTXX_AM230X_MAX_SAMPLES_LEN-1)
		return 1;

	byteindex = 0;
	byte_val = 0;
	byte_mask = 0x80;
	Dhtxx_am230x_Drv.dhtxx_am230x_decoded[byteindex] = 0;
	for(i=initial;i<DHTXX_AM230X_MAX_SAMPLES_LEN;i+=2)
	{
		if (( Dhtxx_am230x_Drv.dhtxx_am230x_samples[i] >= DHTXX_AM230X_50uLOW_MIN ) && ( Dhtxx_am230x_Drv.dhtxx_am230x_samples[i] <= DHTXX_AM230X_50uLOW_MAX ))
		{
			if ( Dhtxx_am230x_Drv.dhtxx_am230x_samples[i+1] >= DHTXX_AM230X_DECODED_1MIN )	//	decoded 1
				byte_val |= byte_mask;
			byte_mask >>= 1;
			if ( byte_mask == 0 )
			{
				Dhtxx_am230x_Drv.dhtxx_am230x_decoded[byteindex] = byte_val;
				Dhtxx_am230x_Drv.checksum += byte_val;
				byteindex ++;
				if ( byteindex == DHTXX_AM230X_BYTES_NUM)
				{
					Dhtxx_am230x_Drv.checksum = 0;
					for(j=0;j<DHTXX_AM230X_BYTES_NUM-1;j++)
						Dhtxx_am230x_Drv.checksum += Dhtxx_am230x_Drv.dhtxx_am230x_decoded[j];
					if ( Dhtxx_am230x_Drv.checksum == Dhtxx_am230x_Drv.dhtxx_am230x_decoded[DHTXX_AM230X_BYTES_NUM-1])
						return 0;
					Dhtxx_am230x_Drv.errors++;
					return 1;
				}
				byte_val = 0;
				byte_mask = 0x80;
			}
		}
	}
	Dhtxx_am230x_Drv.errors++;
	return 1;
}

static void dhtxx_am230x_worker(void)
{
	if ( Dhtxx_am230x_Drv.ticks )
		Dhtxx_am230x_Drv.ticks--;
	switch(Dhtxx_am230x_Drv.state_machine)
	{
	case	DHTXX_AM230X_IDLE:
		if (( Dhtxx_am230x_Drv.status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING)
		{
			Dhtxx_am230x_Drv.state_machine = DHTXX_AM230X_START_BIT_SET;
			Dhtxx_am230x_Drv.ticks = DHTXX_AM230X_START_TICKS;
			GPIO_SetGpioOUT(GPIOPORT_DHTXX_AM230X,GPIOBIT_DHTXX_AM230X,0);
			Dhtxx_am230x_Drv.status |= DHTXX_AM230X_STARTBIT;
		}
		break;
	case	DHTXX_AM230X_START_BIT_SET:
		if ( Dhtxx_am230x_Drv.ticks == 0 )
		{
			Dhtxx_am230x_Drv.status &= ~DHTXX_AM230X_STARTBIT;
			Dhtxx_am230x_Drv.status |= DHTXX_AM230X_ACQRUN;
			GPIO_SetGpioAlternate(GPIOPORT_DHTXX_AM230X,GPIOBIT_DHTXX_AM230X);
			Dhtxx_am230x_Drv.samples_number = 0;
			DHTXX_AM230X_TIMER.Instance->CNT = 0;
			HAL_TIM_IC_Start_DMA(&DHTXX_AM230X_TIMER,TIM_CHANNEL_4, Dhtxx_am230x_Drv.dhtxx_am230x_samples, DHTXX_AM230X_MAX_SAMPLES_LEN);
			Dhtxx_am230x_Drv.state_machine = DHTXX_AM230X_WAIT_FOR_TIM_END;
			Dhtxx_am230x_Drv.ticks = DHTXX_AM230X_CYCLE_TICKS;
		}
		break;
	case	DHTXX_AM230X_WAIT_FOR_TIM_END:
		if ( Dhtxx_am230x_Drv.ticks == 0 )
		{
			HAL_TIM_IC_Stop_DMA(&DHTXX_AM230X_TIMER,TIM_CHANNEL_4);
			GPIO_SetGpioIN(GPIOPORT_DHTXX_AM230X,GPIOBIT_DHTXX_AM230X,1);
			Dhtxx_am230x_Drv.state_machine = DHTXX_AM230X_END;
		}
		break;
	case	DHTXX_AM230X_END:
		Dhtxx_am230x_Drv.status = DHTXX_AM230X_ACQDONE;
		Dhtxx_am230x_Drv.state_machine = DHTXX_AM230X_IDLE;
		if ( dhtxx_am230x_decode() == 0 )
			Dhtxx_am230x_Drv.status |= DHTXX_AM230X_VALID;
		break;
	}
}

void dhtxx_am230x_init(void)
{
	Dhtxx_am230x_Drv.state_machine = DHTXX_AM230X_IDLE;
	Dhtxx_am230x_Drv.ticks = Dhtxx_am230x_Drv.errors = 0;
	set_before_check_timers_callback(dhtxx_am230x_worker);
}

uint8_t dhtxx_am230x_start(void)
{
uint32_t	i;
	if (( Dhtxx_am230x_Drv.status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING )
		return 1;
	for(i=0;i<DHTXX_AM230X_MAX_SAMPLES_LEN  ;i++)
		Dhtxx_am230x_Drv.dhtxx_am230x_samples[i] = 0;
	Dhtxx_am230x_Drv.status = DHTXX_AM230X_RUNNING;
	Dhtxx_am230x_Drv.state_machine = DHTXX_AM230X_IDLE;
	return 0;
}

uint8_t dhtxx_am230x_get_status(void)
{
	return Dhtxx_am230x_Drv.status;
}

uint8_t dhtxx_am230x_get_values(uint8_t *values)
{
uint8_t j;
	if (( Dhtxx_am230x_Drv.status & DHTXX_AM230X_VALID) == DHTXX_AM230X_VALID)
	{
		for(j=0;j<DHTXX_AM230X_BYTES_NUM-1;j++)
			values[j] = Dhtxx_am230x_Drv.dhtxx_am230x_decoded[j];
	}
	else
		return 1;
	return 0;
}

#endif



