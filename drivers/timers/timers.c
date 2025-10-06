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
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#ifdef A_OS_TIMERS_ENABLED
SYSTEM_RAM	TIM_DriverStruct_t	TIM_DriverStruct[MAX_TIM_DRIVERS];
SYSTEM_RAM	uint8_t				last_tim_used_handle=0,tim_driver_request = 0;

extern		Sensors_DriverStruct_t	Sensors_DriverStruct[MAX_I2C_DEVICES];
extern		uint8_t					last_sensor_used_handle,sensor_driver_request;

ITCM_AREA_CODE void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
Dhtxx_am230x_Drv_TypeDef	*dhtxx_am230x_Drv;
Encoder_Drv_TypeDef			*encoder_driver_data;

uint32_t handle_dht,i;
	if ( last_sensor_used_handle )
	{
		for(i=0;i<MAX_DHT11_DEVICES;i++)
		{
			handle_dht = get_handle_from_dht_workers(i);
			if ( handle_dht != 255)
			{
				dhtxx_am230x_Drv = (Dhtxx_am230x_Drv_TypeDef *)Sensors_DriverStruct[handle_dht].private_data;
				dhtxx_am230x_Drv->status = DHTXX_AM230X_ACQDONE;
			}
		}
	}
	if ( last_tim_used_handle )
	{
		for(i=0;i<MAX_TIM_DRIVERS;i++)
		{
			encoder_driver_data = (Encoder_Drv_TypeDef *)TIM_DriverStruct[i].private_data;
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


#endif

