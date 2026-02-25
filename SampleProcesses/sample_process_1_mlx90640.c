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
 * sample_process_1_mlx90640.c
 *
 *  Created on: Feb 25, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_MLX90640

uint16_t			mlx90640_eeprom[MLX90640_EEPROM_DUMP_NUM];
uint16_t 			frame_data[MLX90640_PIXEL_NUM];
uint16_t 			aux_data[MLX90640_AUX_NUM];
float	 			to_data[MLX90640_PIXEL_NUM];
uint16_t 			resulting_image[MLX90640_PIXEL_NUM];
uint16_t 			alpha[MLX90640_PIXEL_NUM];
int16_t 			offset[MLX90640_PIXEL_NUM];
int8_t 				kta[MLX90640_PIXEL_NUM];
int8_t 				kv[MLX90640_PIXEL_NUM];
float 				alphaTemp[MLX90640_PIXEL_NUM];
extern				I2C_HandleTypeDef hi2c2;

I2C_Mlx90640_Drv_TypeDef	I2C_Mlx90640_Drv =
{
		.wakeup_id = 1,
		.bus = &hi2c2,
		.device_address = MLX90640_ADDRESS,
		.mlx90640_eeprom = mlx90640_eeprom,
		.frame_data = frame_data,
		.aux_data = aux_data,
		.to_data = to_data,
		.resulting_image = resulting_image,
		.alpha = alpha,
		.offset = offset,
		.kta = kta,
		.kv = kv,
		.alphaTemp = alphaTemp,
};

void sample_process_1_init(uint32_t process_id)
{
	mlx90640_register(&I2C_Mlx90640_Drv);
}

void sample_process_1_mlx90640(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t	count=0;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			count++;
			if ( count == 10)
			{
				count = 0;
				if  (I2C_Mlx90640_Drv.mlx90640_found )
					mlx90640_run(&I2C_Mlx90640_Drv,1.0,1.0);
			}
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_MLX90640
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
