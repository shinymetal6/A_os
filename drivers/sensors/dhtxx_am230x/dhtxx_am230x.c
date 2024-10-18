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
#include <string.h>

Dhtxx_am230x_Drv_TypeDef	Dhtxx_am230x_Drv;
extern	DriversDefs_t		*DriversDefs[MAX_DRIVERS];

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
TIM_HandleTypeDef *timer = (TIM_HandleTypeDef *)DriversDefs[Dhtxx_am230x_Drv.handle]->peripheral;

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
			timer->Instance->CNT = 0;
			HAL_TIM_IC_Start_DMA(timer,DHTXX_AM230X_TIM_CHANNEL, Dhtxx_am230x_Drv.dhtxx_am230x_samples, DHTXX_AM230X_MAX_SAMPLES_LEN);
			Dhtxx_am230x_Drv.state_machine = DHTXX_AM230X_WAIT_FOR_TIM_END;
			Dhtxx_am230x_Drv.ticks = DHTXX_AM230X_CYCLE_TICKS;
		}
		break;
	case	DHTXX_AM230X_WAIT_FOR_TIM_END:
		if ( Dhtxx_am230x_Drv.ticks == 0 )
		{
			HAL_TIM_IC_Stop_DMA(timer,DHTXX_AM230X_TIM_CHANNEL);
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

static uint32_t dhtxx_am230x_init(uint8_t handle)
{
	Dhtxx_am230x_Drv.state_machine = DHTXX_AM230X_IDLE;
	Dhtxx_am230x_Drv.ticks = Dhtxx_am230x_Drv.errors = 0;
	Dhtxx_am230x_Drv.handle = handle;
	return 0;
}

static uint32_t dhtxx_am230x_start(uint8_t handle)
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

static uint32_t dhtxx_am230x_get_status(uint8_t handle)
{
	return Dhtxx_am230x_Drv.status;
}

static uint32_t dhtxx_am230x_get_values(uint8_t handle,uint8_t *values,uint8_t values_number)
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

extern	DriversDefs_t	dhtxx_am230x_driver_struct;

uint32_t dhtxx_am230x_deinit(uint8_t handle)
{
	return driver_unregister(&dhtxx_am230x_driver_struct);
}

DriversDefs_t	dhtxx_am230x_driver_struct =
{
		.periodic_before_check_timers_callback = dhtxx_am230x_worker,
		.periodic_after_check_timers_callback = NULL,
		.peripheral = (uint32_t *)&DHTXX_AM230X_TIMER,
		.peripheral_channel = DHTXX_AM230X_TIM_CHANNEL,
		.init = dhtxx_am230x_init,
		.deinit = dhtxx_am230x_deinit,
		.start = dhtxx_am230x_start,
		.stop = NULL,
		.get_status = dhtxx_am230x_get_status,
		.get_values = dhtxx_am230x_get_values,
		.driver_name = "dhtxx_am230x",
};

uint32_t dhtxx_get_drv_struct(DriversDefs_t *new_struct,uint8_t peripheral_index)
{
	memcpy(new_struct,&dhtxx_am230x_driver_struct,sizeof(dhtxx_am230x_driver_struct));
	new_struct->peripheral_index = peripheral_index;
	return 0;
}


#endif



