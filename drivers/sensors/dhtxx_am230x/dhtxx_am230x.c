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
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
#include "../../../kernel/kernel_opt.h"

#include "dhtxx_am230x.h"
#include <string.h>

extern		Sensors_DriverStruct_t	Sensors_DriverStruct[MAX_I2C_DEVICES];
extern		uint8_t					last_sensor_used_handle,sensor_driver_request;

ITCM_AREA_CODE static void dhtxx_am230x_create_bitbytes(Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv)
{
uint32_t	i,initial,temp;

	initial =  Dhtxx_am230x_Drv->dhtxx_am230x_samples[0];
	for(i=0;i<DHTXX_AM230X_MAX_SAMPLES_LEN-1;i++)
	{
		temp = Dhtxx_am230x_Drv->dhtxx_am230x_samples[i+1];
		Dhtxx_am230x_Drv->dhtxx_am230x_samples[i+1] -= initial;
		initial = temp;
	}
}

ITCM_AREA_CODE static uint8_t dhtxx_am230x_decode(Dhtxx_am230x_Drv_TypeDef	*Dhtxx_am230x_Drv)
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

ITCM_AREA_CODE static uint32_t get_handle_from_dht_workers(uint32_t device_index)
{
uint32_t	i,drv_ret=255;
	for(i=0;i<MAX_I2C_DEVICES;i++)
	{
		if ( Sensors_DriverStruct[i].sensor_id == device_index )
			return i;
	}
	return drv_ret;
}

ITCM_AREA_CODE static void dhtxx_am230x_worker(void)
{
Dhtxx_am230x_Drv_TypeDef	*dhtxx_am230x_Drv;
TIM_HandleTypeDef 			*dht_timer;
uint16_t 					dht_timer_channel;
uint32_t 					i,handle_dht;

	for(i=0;i<MAX_DHT11_DEVICES;i++)
	{
		handle_dht = get_handle_from_dht_workers(i);
		if ( handle_dht != 255)
		{
			dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)Sensors_DriverStruct[handle_dht].sensor_driver_private_data;
			dht_timer = dhtxx_am230x_Drv->dht_timer;
			dht_timer_channel = dhtxx_am230x_Drv->dht_timer_channel;

			if ( dhtxx_am230x_Drv->ticks )
				dhtxx_am230x_Drv->ticks--;
			switch(dhtxx_am230x_Drv->state_machine)
			{
			case	DHTXX_AM230X_IDLE:
				if (( dhtxx_am230x_Drv->status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING)
				{
					dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_START_BIT_SET;
					dhtxx_am230x_Drv->ticks = DHTXX_AM230X_START_TICKS;
					set_gpio_mode(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,GPIO_IS_OUTPUT,0);
					dhtxx_am230x_Drv->status |= DHTXX_AM230X_STARTBIT;
				}
				break;
			case	DHTXX_AM230X_START_BIT_SET:
				if ( dhtxx_am230x_Drv->ticks == 0 )
				{
					dhtxx_am230x_Drv->status &= ~DHTXX_AM230X_STARTBIT;
					dhtxx_am230x_Drv->status |= DHTXX_AM230X_ACQRUN;
					set_gpio_mode(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,GPIO_IS_ALTERNATE,0);					dhtxx_am230x_Drv->samples_number = 0;
					dht_timer->Instance->CNT = 0;
					HAL_TIM_IC_Start_DMA(dht_timer,dht_timer_channel, dhtxx_am230x_Drv->dhtxx_am230x_samples, DHTXX_AM230X_MAX_SAMPLES_LEN);
					dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_WAIT_FOR_TIM_END;
					dhtxx_am230x_Drv->ticks = DHTXX_AM230X_CYCLE_TICKS;
				}
				break;
			case	DHTXX_AM230X_WAIT_FOR_TIM_END:
				if ( dhtxx_am230x_Drv->ticks == 0 )
				{
					HAL_TIM_IC_Stop_DMA(dht_timer,dht_timer_channel);
					set_gpio_mode(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,GPIO_IS_INPUT,0);					dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_END;
				}
				break;
			case	DHTXX_AM230X_END:
				dhtxx_am230x_Drv->status = DHTXX_AM230X_ACQDONE;
				dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
				if ( dhtxx_am230x_decode(dhtxx_am230x_Drv) == 0 )
					dhtxx_am230x_Drv->status |= DHTXX_AM230X_VALID;
				break;
			}
		}
	}
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_init(uint8_t handle_dht)
{
	Dhtxx_am230x_Drv_TypeDef	*dhtxx_am230x_Drv;
	dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)Sensors_DriverStruct[handle_dht].sensor_driver_private_data;

	dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
	dhtxx_am230x_Drv->ticks = dhtxx_am230x_Drv->errors = 0;

	set_before_check_timers_callback(dhtxx_am230x_worker);

	return 0;
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_start(uint8_t handle_dht)
{
uint32_t	i;
Dhtxx_am230x_Drv_TypeDef	*dhtxx_am230x_Drv;
	dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)Sensors_DriverStruct[handle_dht].sensor_driver_private_data;

	if (( dhtxx_am230x_Drv->status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING )
		return 1;
	for(i=0;i<DHTXX_AM230X_MAX_SAMPLES_LEN  ;i++)
		dhtxx_am230x_Drv->dhtxx_am230x_samples[i] = 0;
	dhtxx_am230x_Drv->status = DHTXX_AM230X_RUNNING;
	dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
	return 0;
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_get_values(uint8_t handle_dht,uint8_t *values)
{
uint8_t j;
Dhtxx_am230x_Drv_TypeDef	*dhtxx_am230x_Drv;
	dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)Sensors_DriverStruct[handle_dht].sensor_driver_private_data;

	if (( dhtxx_am230x_Drv->status & DHTXX_AM230X_VALID) == DHTXX_AM230X_VALID)
	{
		for(j=0;j<DHTXX_AM230X_BYTES_NUM-1;j++)
			values[j] = dhtxx_am230x_Drv->dhtxx_am230x_decoded[j];
	}
	else
		return 0;
	return DHTXX_AM230X_BYTES_NUM-1;
}

ITCM_AREA_CODE uint32_t	dhtxx_am230x_register(Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_driver_private_data,uint32_t driver_flags,uint32_t dhtxx_am230x_flags,uint32_t sensor_id)
{
Dhtxx_am230x_Drv_TypeDef	*dhtxx_am230x_Drv;
	if ( Sensors_DriverStruct[last_sensor_used_handle].process == 0 )
	{
		Sensors_DriverStruct[last_sensor_used_handle].process = get_current_process();
		Sensors_DriverStruct[last_sensor_used_handle].flags |= driver_flags;
		Sensors_DriverStruct[last_sensor_used_handle].sensor_driver_private_data = (uint32_t *)dhtxx_am230x_driver_private_data;

		dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)Sensors_DriverStruct[last_sensor_used_handle].sensor_driver_private_data;
		if (( dhtxx_am230x_Drv->dht_timer != NULL ) && ( dhtxx_am230x_Drv->one_wire_port != NULL ))
		{
			dhtxx_am230x_Drv->flags |= dhtxx_am230x_flags;
			Sensors_DriverStruct[last_sensor_used_handle].sensor_id = sensor_id;
			Sensors_DriverStruct[last_sensor_used_handle].status = DRIVER_STATUS_IN_USE;

			last_sensor_used_handle++;
			sensor_driver_request++;
			return last_sensor_used_handle-1;
		}
	}
	return DRIVER_REQUEST_FAILED;
}

/*********** Interrupt *********/


ITCM_AREA_CODE void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
Dhtxx_am230x_Drv_TypeDef	*dhtxx_am230x_Drv;
uint32_t handle_dht,i;
	for(i=0;i<MAX_DHT11_DEVICES;i++)
	{
		handle_dht = get_handle_from_dht_workers(i);
		if ( handle_dht != 255)
		{
			dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)Sensors_DriverStruct[handle_dht].sensor_driver_private_data;
			dhtxx_am230x_Drv->status = DHTXX_AM230X_ACQDONE;
		}
	}
}

