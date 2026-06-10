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
 * sample_process_1_tof_vl53l5cx.c
 *
 *  Created on: Feb 27, 2026
 *      Author: fil
 */
#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_TOF_VL53L5CX
extern	I2C_HandleTypeDef hi2c2;

VL53L5CX_ResultsData_TypeDef	VL53L5CX_ResultsData =
{

};
I2C_vl53l5cx_DriverStruct_t I2C_vl53l5cx_Drv =
{
		.bus = &hi2c2,
		.lpn_port = TOF_LPN_GPIO_Port,
		.lpn_bit = TOF_LPN_Pin,
		.wakeup_id = WAKEUP_FROM_I2C2_IRQ,
		.VL53L5CX_ResultsData = &VL53L5CX_ResultsData,
};

void sample_process_1_init(uint32_t process_id)
{
	vl53l5cx_register(&I2C_vl53l5cx_Drv);
}

void sample_process_1_tof_vl53l5cx(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		count=0;

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	vl53l5cx_start_ranging(&I2C_vl53l5cx_Drv);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			count++;
			if ( count > 9 )
			{
				vl53l5cx_get_ranging_data(&I2C_vl53l5cx_Drv);
				count=0;
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_TOF_VL53L5CX
#endif // #ifdef SAMPLE_PROCESSES_ENABLED




