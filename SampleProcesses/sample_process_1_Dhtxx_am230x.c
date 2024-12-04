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
 * sample_process_1_Dhtxx_am230x.c
 *
 *  Created on: Dec 4, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef 	LD3_GPIO_Port
#define	DHT11_SENSOR_ID	0xdeadbeef
extern	TIM_HandleTypeDef htim2;

Dhtxx_am230x_Drv_TypeDef	Dhtxx_am230x_Drv =
{
		.device_index = 0x00,
		.dht_timer = &htim2,
		.dht_timer_channel = TIM_CHANNEL_4,
		.one_wire_port = GPIOA,
		.one_wire_bit = 3,
};
uint32_t		dht_driver_handle;
uint8_t			dht_data[DHTXX_AM230X_BYTES_NUM];
uint8_t dht_sm = 0;

void sample_process_1_Dhtxx_am230x(uint32_t process_id)
{
uint32_t	wakeup,flags;
	dht_driver_handle = dhtxx_am230x_register(&Dhtxx_am230x_Drv,0,0,Dhtxx_am230x_Drv.device_index);
	dhtxx_am230x_init(dht_driver_handle);
	dhtxx_am230x_start(dht_driver_handle);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_SET);

	create_timer(TIMER_ID_0,500,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			switch ( dht_sm)
			{
			case 0 :
				dhtxx_am230x_start(dht_driver_handle);
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
				dht_sm++;
				break;
			case 1 :
				if ( dhtxx_am230x_get_values(dht_driver_handle,dht_data) == 0 )
				{
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_SET);
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
				}
				else
				{
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin,GPIO_PIN_RESET);
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
				}
				dht_sm = 0;
				break;

			}
		}
	}
}
#else
void sample_process_1_Dhtxx_am230x(uint32_t process_id)
{
	wait_event(HW_SLEEP_FOREVER);
}
#endif // #ifdef 	LD3_GPIO_Port

#endif // #ifdef SAMPLE_PROCESSES_ENABLED

