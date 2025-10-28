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

ITCM_AREA_CODE uint32_t dhtxx_am230x_decode(Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv)
{
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

Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Running;
uint32_t	ret_icval;

ITCM_AREA_CODE static void dhtxx_am230x_worker(void)
{
uint32_t 	i;

	for(i=0;i<MAX_DHT11_DEVICES;i++)
	{
		Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = dhtxx_am230x_Running;
		if ( dhtxx_am230x_Drv != NULL)
		{
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
					ret_icval = HAL_TIM_IC_Start_DMA(dhtxx_am230x_Drv->timer,dhtxx_am230x_Drv->dht_timer_channel, (uint32_t *)dhtxx_am230x_Drv->dhtxx_am230x_samples, DHTXX_AM230X_SAMPLESLEN);
					dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_WAIT_FOR_TIM_END;
					dhtxx_am230x_Drv->ticks = DHTXX_AM230X_CYCLE_TICKS;
				}
				break;
			case	DHTXX_AM230X_WAIT_FOR_TIM_END:
				if ( dhtxx_am230x_Drv->ticks == 0 )
				{
					HAL_TIM_IC_Stop_DMA(dhtxx_am230x_Drv->timer,dhtxx_am230x_Drv->dht_timer_channel);
					set_gpio_mode(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,MODE_INPUT,0);
					dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_END;
				}
				break;
			case	DHTXX_AM230X_END:
				dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
				if ( dhtxx_am230x_decode(dhtxx_am230x_Drv) == 0 )
					dhtxx_am230x_Drv->status = DHTXX_AM230X_ACQDONE;
				break;
			}
		}
	}
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_init(Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv)
{
	dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
	dhtxx_am230x_Drv->ticks = 0;
	set_gpio_type(dhtxx_am230x_Drv->one_wire_port,dhtxx_am230x_Drv->one_wire_bit,OUTPUT_OD,GPIO_PULLUP,GPIO_SPEED_FREQ_HIGH );
	set_before_check_timers_callback(dhtxx_am230x_worker);
	return 0;
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_start(Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv)
{
	if (( dhtxx_am230x_Drv->status & DHTXX_AM230X_RUNNING) == DHTXX_AM230X_RUNNING )
		return 1;
	dhtxx_am230x_Drv->status = DHTXX_AM230X_RUNNING;
	dhtxx_am230x_Drv->state_machine = DHTXX_AM230X_IDLE;
	dhtxx_am230x_Running = dhtxx_am230x_Drv;
	return 0;
}

ITCM_AREA_CODE uint32_t dhtxx_am230x_get_status(Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv)
{
	if (( dhtxx_am230x_Drv->status & DHTXX_AM230X_ACQDONE) == DHTXX_AM230X_ACQDONE)
		return 0;
	return 1;
}

ITCM_AREA_CODE uint32_t	dhtxx_am230x_register(Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_drv)
{

TIMER_DriverStruct_t *eptr, *pre_eptr;

	if ( dhtxx_am230x_drv->timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( timer_drv_ptr == NULL)
	{
		timer_drv_ptr = (TIMER_DriverStruct_t *)dhtxx_am230x_drv;
		dhtxx_am230x_drv->next_timer = NULL;
	}
	else
	{
		eptr = pre_eptr = timer_drv_ptr;
		while(eptr->next_timer != NULL)
		{
			pre_eptr = eptr;
			eptr = (TIMER_DriverStruct_t *)eptr->next_timer;
		}
		pre_eptr->next_timer = (uint32_t *)dhtxx_am230x_drv;
		dhtxx_am230x_drv->next_timer = NULL;
	}
	return 0;
}

#endif // #ifdef A_OS_TIMERS_ENABLED
