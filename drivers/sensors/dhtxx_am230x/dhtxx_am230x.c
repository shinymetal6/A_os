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

extern	DriverStruct_t		*DriverStruct[MAX_DRIVERS];
static uint32_t dhtxx_am230x_init(uint8_t handle_dht);

static uint32_t get_handle_from_dht_workers(uint32_t device_index)
{
uint32_t	i,drv_ret=255;
	for(i=0;i<MAX_DRIVERS;i++)
	{
		if ( DriverStruct[i] != NULL )
		{
			if (( DriverStruct[i]->status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
			{
				if ( DriverStruct[i]->driver_private_data != NULL)
				{
					if ( DriverStruct[i]->init == dhtxx_am230x_init)
					{
						Dhtxx_am230x_Drv_TypeDef *Dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)DriverStruct[i]->driver_private_data;
						if ( Dhtxx_am230x_Drv->device_index == device_index )
							return Dhtxx_am230x_Drv->device_index;
					}
				}
			}
		}
	}
	return drv_ret;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv;
uint32_t handle_dht,i;
	for(i=0;i<MAX_DHT11_DEVICES;i++)
	{
		handle_dht = get_handle_from_dht_workers(i);
		if ( handle_dht != 255)
		{
			Dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef	*)DriverStruct[handle_dht]->driver_private_data;
			Dhtxx_am230x_Drv->status = DHTXX_AM230X_ACQDONE;
		}
	}
}

static void dhtxx_am230x_create_bitbytes(Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv)
{
uint32_t	i,initial,temp;

	initial =  Dhtxx_am230x_Drv->dhtxx_am230x_samples[0];
	for(i=0;i<DHTXX_AM230X_MAX_SAMPLES_LEN;i++)
	{
		temp = Dhtxx_am230x_Drv->dhtxx_am230x_samples[i+1];
		Dhtxx_am230x_Drv->dhtxx_am230x_samples[i+1] -= initial;
		initial = temp;
	}
}

static uint8_t dhtxx_am230x_decode(Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv)
{
uint32_t	i,j,initial,byteindex;
uint8_t		byte_val,byte_mask;

	dhtxx_am230x_create_bitbytes(Dhtxx_am230x_Drv);

	for(i=0;i<DHTXX_AM230X_MAX_SAMPLES_LEN;i++)
	{
		if (( Dhtxx_am230x_Drv->dhtxx_am230x_samples[i] > DHTXX_AM230X_START_MINIMUM) && ( Dhtxx_am230x_Drv->dhtxx_am230x_samples[i] < DHTXX_AM230X_START_MAXIMUM))
		{
			if (( Dhtxx_am230x_Drv->dhtxx_am230x_samples[i+1] > DHTXX_AM230X_START_MINIMUM) && ( Dhtxx_am230x_Drv->dhtxx_am230x_samples[i+1] < DHTXX_AM230X_START_MAXIMUM))
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
	Dhtxx_am230x_Drv->dhtxx_am230x_decoded[byteindex] = 0;
	for(i=initial;i<DHTXX_AM230X_MAX_SAMPLES_LEN;i+=2)
	{
		if (( Dhtxx_am230x_Drv->dhtxx_am230x_samples[i] >= DHTXX_AM230X_50uLOW_MIN ) && ( Dhtxx_am230x_Drv->dhtxx_am230x_samples[i] <= DHTXX_AM230X_50uLOW_MAX ))
		{
			if ( Dhtxx_am230x_Drv->dhtxx_am230x_samples[i+1] >= DHTXX_AM230X_DECODED_1MIN )	//	decoded 1
				byte_val |= byte_mask;
			byte_mask >>= 1;
			if ( byte_mask == 0 )
			{
				Dhtxx_am230x_Drv->dhtxx_am230x_decoded[byteindex] = byte_val;
				Dhtxx_am230x_Drv->checksum += byte_val;
				byteindex ++;
				if ( byteindex == DHTXX_AM230X_BYTES_NUM)
				{
					Dhtxx_am230x_Drv->checksum = 0;
					for(j=0;j<DHTXX_AM230X_BYTES_NUM-1;j++)
						Dhtxx_am230x_Drv->checksum += Dhtxx_am230x_Drv->dhtxx_am230x_decoded[j];
					if ( Dhtxx_am230x_Drv->checksum == Dhtxx_am230x_Drv->dhtxx_am230x_decoded[DHTXX_AM230X_BYTES_NUM-1])
						return 0;
					Dhtxx_am230x_Drv->errors++;
					return 1;
				}
				byte_val = 0;
				byte_mask = 0x80;
			}
		}
	}
	Dhtxx_am230x_Drv->errors++;
	return 1;
}

static uint32_t dhtxx_am230x_init(uint8_t handle_dht)
{
Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv;
Dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef	*)DriverStruct[handle_dht]->driver_private_data;
	Dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
	Dhtxx_am230x_Drv->ticks = Dhtxx_am230x_Drv->errors = 0;
	return 0;
}

static void dhtxx_am230x_worker(void)
{
Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv;
TIM_HandleTypeDef 			*dht_timer;
uint16_t 					dht_timer_channel;
uint32_t 					i,handle_dht;

	for(i=0;i<MAX_DHT11_DEVICES;i++)
	{
		handle_dht = get_handle_from_dht_workers(i);
		if ( handle_dht != 255)
		{
			Dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef	*)DriverStruct[handle_dht]->driver_private_data;
			dht_timer = Dhtxx_am230x_Drv->dht_timer;
			dht_timer_channel = Dhtxx_am230x_Drv->dht_timer_channel;

			if ( Dhtxx_am230x_Drv->ticks )
				Dhtxx_am230x_Drv->ticks--;
			switch(Dhtxx_am230x_Drv->state_machine)
			{
			case	DHTXX_AM230X_IDLE:
				if (( Dhtxx_am230x_Drv->status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING)
				{
					Dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_START_BIT_SET;
					Dhtxx_am230x_Drv->ticks = DHTXX_AM230X_START_TICKS;
					GPIO_SetGpioOUT(Dhtxx_am230x_Drv->one_wire_port,Dhtxx_am230x_Drv->one_wire_bit,0);
					Dhtxx_am230x_Drv->status |= DHTXX_AM230X_STARTBIT;
				}
				break;
			case	DHTXX_AM230X_START_BIT_SET:
				if ( Dhtxx_am230x_Drv->ticks == 0 )
				{
					Dhtxx_am230x_Drv->status &= ~DHTXX_AM230X_STARTBIT;
					Dhtxx_am230x_Drv->status |= DHTXX_AM230X_ACQRUN;
					GPIO_SetGpioAlternate(Dhtxx_am230x_Drv->one_wire_port,Dhtxx_am230x_Drv->one_wire_bit);
					Dhtxx_am230x_Drv->samples_number = 0;
					dht_timer->Instance->CNT = 0;
					HAL_TIM_IC_Start_DMA(dht_timer,dht_timer_channel, Dhtxx_am230x_Drv->dhtxx_am230x_samples, DHTXX_AM230X_MAX_SAMPLES_LEN);
					Dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_WAIT_FOR_TIM_END;
					Dhtxx_am230x_Drv->ticks = DHTXX_AM230X_CYCLE_TICKS;
				}
				break;
			case	DHTXX_AM230X_WAIT_FOR_TIM_END:
				if ( Dhtxx_am230x_Drv->ticks == 0 )
				{
					HAL_TIM_IC_Stop_DMA(dht_timer,dht_timer_channel);
					GPIO_SetGpioIN(Dhtxx_am230x_Drv->one_wire_port,Dhtxx_am230x_Drv->one_wire_bit,1);
					Dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_END;
				}
				break;
			case	DHTXX_AM230X_END:
				Dhtxx_am230x_Drv->status = DHTXX_AM230X_ACQDONE;
				Dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
				if ( dhtxx_am230x_decode(Dhtxx_am230x_Drv) == 0 )
					Dhtxx_am230x_Drv->status |= DHTXX_AM230X_VALID;
				break;
			}
		}
	}
}

static uint32_t dhtxx_am230x_start(uint8_t handle_dht)
{
uint32_t	i;
Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv;
Dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef	*)DriverStruct[handle_dht]->driver_private_data;

	if (( Dhtxx_am230x_Drv->status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING )
		return 1;
	for(i=0;i<DHTXX_AM230X_MAX_SAMPLES_LEN  ;i++)
		Dhtxx_am230x_Drv->dhtxx_am230x_samples[i] = 0;
	Dhtxx_am230x_Drv->status = DHTXX_AM230X_RUNNING;
	Dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
	return 0;
}

static uint32_t dhtxx_am230x_get_status(uint8_t handle_dht)
{
Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv;
Dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef	*)DriverStruct[handle_dht]->driver_private_data;
	return Dhtxx_am230x_Drv->status;
}

static uint32_t dhtxx_am230x_get_values(uint8_t handle_dht,uint8_t *values,uint8_t values_number)
{
uint8_t j;
Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv;
Dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef	*)DriverStruct[handle_dht]->driver_private_data;

	if (( Dhtxx_am230x_Drv->status & DHTXX_AM230X_VALID) == DHTXX_AM230X_VALID)
	{
		for(j=0;j<DHTXX_AM230X_BYTES_NUM-1;j++)
			values[j] = Dhtxx_am230x_Drv->dhtxx_am230x_decoded[j];
	}
	else
		return 1;
	return 0;
}

extern	const DriverStruct_t	dhtxx_am230x_driver_struct;

uint32_t dhtxx_am230x_deinit(uint8_t handle)
{
	return driver_unregister((DriverStruct_t *)&dhtxx_am230x_driver_struct);
}

const DriverStruct_t	dhtxx_am230x_driver_struct =
{
	.periodic_before_check_timers_callback = dhtxx_am230x_worker,
	.periodic_after_check_timers_callback = NULL,
	.init = dhtxx_am230x_init,
	.deinit = dhtxx_am230x_deinit,
	.start = dhtxx_am230x_start,
	.stop = NULL,
	.get_status = dhtxx_am230x_get_status,
	.get_values = dhtxx_am230x_get_values,
	.driver_name = "dhtxx_am230x",
};

uint32_t dhtxx_allocate_driver(DriverStruct_t *new_struct)
{
	memcpy(new_struct,&dhtxx_am230x_driver_struct,sizeof(dhtxx_am230x_driver_struct));
	return 0;
}


#endif



