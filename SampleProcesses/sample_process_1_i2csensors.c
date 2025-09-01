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
 * sample_process_1_i2csensors.c
 *
 *  Created on: Dec 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef	SAMPLEPROCESS_I2CSENSORS
extern	I2C_HandleTypeDef hi2c1;

uint8_t	sht40_data[SHT40_LEN];
uint8_t	lps22_data[LPS22DF_P_LEN];

I2C_Sensors_DriverStruct_t	sht40_drv =
{
	.sensor_id = 0x01,
	.bus = &hi2c1,
	.device_address = SHT40_ADDR,
	.data = sht40_data,
	.wakeup_id = WAKEUP_FROM_I2C1_IRQ,
	.power_port = SENSORS_POWER_GPIO_Port,
	.power_bit = SENSORS_POWER_Pin,
	.power_active_level = 1,
};
uint32_t		sht40_drv_handle;


I2C_Sensors_DriverStruct_t	lps22_drv =
{
	.sensor_id = 0x01,
	.bus = &hi2c1,
	.device_address = LPS22DF_ADDR,
	.data = lps22_data,
	.wakeup_id = WAKEUP_FROM_I2C1_IRQ,
	.power_port = SENSORS_POWER_GPIO_Port,
	.power_bit = SENSORS_POWER_Pin,
	.power_active_level = 1,
};
uint32_t		lps22_drv_handle;

void sample_process_1_i2csensors(uint32_t process_id)
{
uint32_t	wakeup,flags;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	sht40_drv_handle = sht40_register(&sht40_drv);
	lps22_drv_handle = sht40_register(&lps22_drv);
	sensor_power_on(sht40_drv_handle);
	sensor_init(sht40_drv_handle);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			sensor_get_data(sht40_drv_handle);
			sensor_get_data(lps22_drv_handle);
			sensor_start(sht40_drv_handle);
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_I2C
#endif // #ifdef SAMPLE_PROCESSES_ENABLED




