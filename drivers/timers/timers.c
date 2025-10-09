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
 * timers.c
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef A_OS_TIMERS_ENABLED
SYSTEM_RAM	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
SYSTEM_RAM	uint8_t				last_tim_used_handle=0,tim_driver_request = 0;

ITCM_AREA_CODE void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
uint32_t i,j;

	if ( last_tim_used_handle )
	{
		for(i=0;i<MAX_TIM_DRIVERS;i++)
		{
			if ( TIM_DriverStruct[i].timer_type == TIM_TYPE_DHT11 )
			{
				for(j=0;j<MAX_DHT11_DEVICES;j++)
				{
					uint32_t handle_dht = get_handle_from_dht_workers(j);
					if ( handle_dht != 255)
					{
						Dhtxx_am230x_Drv_TypeDef *dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)TIM_DriverStruct[handle_dht].private_data;
						dhtxx_am230x_Drv->status = DHTXX_AM230X_ACQDONE;
					}
				}
			}
			if ( TIM_DriverStruct[i].timer_type == TIM_TYPE_ENCODER )
			{
				Encoder_Drv_TypeDef	*encoder_driver_data = (Encoder_Drv_TypeDef *)TIM_DriverStruct[i].private_data;
				if ( encoder_driver_data->encoder_timer == htim)
				{
					encoder_driver_data->encoder_value = (htim->Instance->CNT>>2);
					if ( encoder_driver_data->encoder_value != encoder_driver_data->encoder_last_value )
					{
						encoder_driver_data->status &= ~(ENCODER_UP | ENCODER_DOWN);

						if ( encoder_driver_data->encoder_value > encoder_driver_data->encoder_last_value)
							encoder_driver_data->status |= ENCODER_UP;
						else
							encoder_driver_data->status |= ENCODER_DOWN;
						encoder_driver_data->status |= ENCODER_READY;
						encoder_driver_data->encoder_last_value = encoder_driver_data->encoder_value;
						if ( encoder_driver_data->irq_encoder_callback != NULL )
							encoder_driver_data->irq_encoder_callback(encoder_driver_data->encoder_value);
						if ( encoder_driver_data->wakeup_id )
							activate_process(TIM_DriverStruct[i].process,encoder_driver_data->wakeup_id,0x01);
					}
				}
			}
		}
	}
}


#endif

