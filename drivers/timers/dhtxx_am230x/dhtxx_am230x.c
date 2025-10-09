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

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_TIMERS_ENABLED

#include "dhtxx_am230x.h"
#include <string.h>

extern	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
extern	uint8_t				last_tim_used_handle,tim_driver_request;

extern	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
extern	uint8_t				last_tim_used_handle,tim_driver_request;

ITCM_AREA_CODE uint32_t get_handle_from_dht_workers(uint32_t sensor_id)
{
uint32_t	i,drv_ret=255;

	for(i=0;i<MAX_TIM_DRIVERS;i++)
	{
		Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)TIM_DriverStruct[i].private_data;
		if ( dhtxx_am230x_Drv->sensor_id == sensor_id )
			return i;
	}
	return drv_ret;
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_decode(uint8_t handle_dht)
{
Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)TIM_DriverStruct[handle_dht].private_data;
uint16_t i,j,tmplen;
uint8_t idx , n;
	for(i=3,j=0;i<DHTXX_AM230X_SAMPLESLEN;i++)
	{
		tmplen = dhtxx_am230x_Drv->dhtxx_am230x_samples[i] - dhtxx_am230x_Drv->dhtxx_am230x_samples[i-1];
		if ( tmplen > DHTXX_AM230X_PULSE_END)
			break;
		if (( tmplen < DHTXX_AM230X_0_VAL ) || ( tmplen > DHTXX_AM230X_1_VAL ))
		{
			dhtxx_am230x_Drv->dhtxx_am230x_length[j] = (tmplen > DHTXX_AM230X_1_VAL) ? 1 : 0;
			j++;
		}
	}
	dhtxx_am230x_Drv->dhtxx_data[0] = n = 0;
	for(i=0,idx=7;i<DHTXX_AM230X_BITNUM;i++,idx--)
	{
		dhtxx_am230x_Drv->dhtxx_data[n] |= dhtxx_am230x_Drv->dhtxx_am230x_length[i]<<idx;
		if ( idx == 0)
		{
			idx = 8;
			n++;
			dhtxx_am230x_Drv->dhtxx_data[n] = 0;
		}
	}
	dhtxx_am230x_Drv->checksum = dhtxx_am230x_Drv->dhtxx_data[0] + dhtxx_am230x_Drv->dhtxx_data[1] + dhtxx_am230x_Drv->dhtxx_data[2] + dhtxx_am230x_Drv->dhtxx_data[3];
	if (dhtxx_am230x_Drv->dhtxx_data[4] != dhtxx_am230x_Drv->checksum)
		return 1; // Checksum error
	return 0;
}

uint32_t ret_icval;
ITCM_AREA_CODE static void dhtxx_am230x_worker(void)
{
uint32_t 					i,handle_dht;

	for(i=0;i<MAX_DHT11_DEVICES;i++)
	{
		handle_dht = get_handle_from_dht_workers(i);
		if ( handle_dht != 255)
		{
			Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)TIM_DriverStruct[handle_dht].private_data;

			if ( dhtxx_am230x_Drv->ticks )
				dhtxx_am230x_Drv->ticks--;
			switch(dhtxx_am230x_Drv->state_machine)
			{
			case	DHTXX_AM230X_IDLE:
				if (( dhtxx_am230x_Drv->status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING)
				{
					dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_START_BIT_SET;
					dhtxx_am230x_Drv->ticks = DHTXX_AM230X_START_TICKS;
					set_gpio_mode(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,MODE_OUTPUT,0);
				}
				break;
			case	DHTXX_AM230X_START_BIT_SET:
				if ( dhtxx_am230x_Drv->ticks == 0 )
				{
					set_gpio_mode(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,MODE_AF,0);
					ret_icval = HAL_TIM_IC_Start_DMA(dhtxx_am230x_Drv->dht_timer,dhtxx_am230x_Drv->dht_timer_channel, (uint32_t *)dhtxx_am230x_Drv->dhtxx_am230x_samples, DHTXX_AM230X_SAMPLESLEN);
					dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_WAIT_FOR_TIM_END;
					dhtxx_am230x_Drv->ticks = DHTXX_AM230X_CYCLE_TICKS;
				}
				break;
			case	DHTXX_AM230X_WAIT_FOR_TIM_END:
				if ( dhtxx_am230x_Drv->ticks == 0 )
				{
					HAL_TIM_IC_Stop_DMA(dhtxx_am230x_Drv->dht_timer,dhtxx_am230x_Drv->dht_timer_channel);
					set_gpio_mode(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,MODE_INPUT,0);
					dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_END;
				}
				break;
			case	DHTXX_AM230X_END:
				dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
				if ( dhtxx_am230x_decode(handle_dht) == 0 )
					dhtxx_am230x_Drv->status = DHTXX_AM230X_ACQDONE;
				break;
			}
		}
	}
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_init(uint8_t handle_dht)
{
Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)TIM_DriverStruct[handle_dht].private_data;

	dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
	dhtxx_am230x_Drv->ticks = 0;
	set_gpio_type(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,OUTPUT_OD,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH );
	set_before_check_timers_callback(dhtxx_am230x_worker);
	return 0;
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_start(uint8_t handle_dht)
{
Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)TIM_DriverStruct[handle_dht].private_data;

	if (( dhtxx_am230x_Drv->status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING )
		return 1;
	dhtxx_am230x_Drv->status = DHTXX_AM230X_RUNNING;
	dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
	return 0;
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_get_status(uint8_t handle_dht)
{
Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)TIM_DriverStruct[handle_dht].private_data;
	if (( dhtxx_am230x_Drv->status & DHTXX_AM230X_ACQDONE) == DHTXX_AM230X_ACQDONE)
		return 0;
	return 1;
}

ITCM_AREA_CODE uint32_t	dhtxx_am230x_register(Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_driver_private_data)
{
	if ( TIM_DriverStruct[last_tim_used_handle].process == 0 )
	{
		TIM_DriverStruct[last_tim_used_handle].process = get_current_process();
		TIM_DriverStruct[last_tim_used_handle].private_data = (uint32_t *)dhtxx_am230x_driver_private_data;

		Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)TIM_DriverStruct[last_tim_used_handle].private_data;
		if (( dhtxx_am230x_Drv->dht_timer != NULL ) && ( dhtxx_am230x_Drv->one_wire_port != NULL ))
		{
			dhtxx_am230x_Drv->status = DRIVER_STATUS_IN_USE;
			TIM_DriverStruct[last_tim_used_handle].timer_type = TIM_TYPE_DHT11;
			last_tim_used_handle++;
			tim_driver_request++;
			return last_tim_used_handle-1;
		}
	}
	return DRIVER_REQUEST_FAILED;
}

#endif // #ifdef A_OS_TIMERS_ENABLED
