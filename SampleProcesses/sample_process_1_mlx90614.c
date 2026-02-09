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
 * sample_process_1_mlx90614.c
 *
 *  Created on: Feb 9, 2026
 *      Author: fil
 */
/*
 * Pin allocation for NUCLEO-G491RE
 * I2C2_SCL		PA9		CN10-21
 * I2C2_SDA		PA8		CN10-23
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_MLX90614
extern	I2C_HandleTypeDef hi2c2;

I2C_Mlx90614_Drv_TypeDef	mlx90614_Drv =
{
		.wakeup_id = 1,
		.bus = &hi2c2,
		.device_address = MLX90614_I2C_ADDR,
};

void sample_process_1_init(uint32_t process_id)
{

}


void sample_process_1_mlx90614(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t counter = 0;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	mlx90614_register(&mlx90614_Drv);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			counter++;
			if (counter == 10)
				mlx90614_read_ambient(&mlx90614_Drv);
			if (counter == 20)
				mlx90614_read_object(&mlx90614_Drv);
			if (counter == 21)
				counter = 1;

		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_MLX90614
#endif // #ifdef SAMPLE_PROCESSES_ENABLED

