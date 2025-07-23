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
 * sample_process_1_i2cmem.c
 *
 *  Created on: Dec 4, 2024
 *      Author: fil
 */

#include "main.h"
#include "A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"

#ifdef AOS_STM32H7xx_HAL_I2C_H

extern	I2C_HandleTypeDef hi2c1;

#define	I2CMEM_BUFFERSIZE	I2C_24XX_PAGESIZE*4

uint8_t	i2cBufw[I2CMEM_BUFFERSIZE];
uint8_t	i2cBufr[I2CMEM_BUFFERSIZE];

ExtFlash_DriverStruct_t	i2c_24xx_driver;

I2C_24xx_Drv_TypeDef	i2c_24xx_Drv =
{
		.bus = &hi2c1,
		.i2c_scl_port = GPIOB,
		.i2c_scl_bit = 8,
		.device_address = I2C_24XX_ADDRESS,
		.device_address_size = I2C_MEMADD_SIZE_16BIT,
		.device_size = 65536,
		.flags = I2C_FLAGS_USES_READ_DMA | I2C_FLAGS_USES_WRITE_DMA | I2C_FLAGS_WAKEUP_ON_READ | I2C_FLAGS_WAKEUP_ON_WRITE | I2C_FLAGS_WAIT_ON_WRITE_COMPLETE | I2C_FLAGS_WAIT_ON_READ_COMPLETE,
		.wakeup_id = WAKEUP_FROM_I2C1_IRQ,
};
uint32_t		i2cflash_driver_handle;

void sample_process_1_i2cmem(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint32_t	i;

	create_timer(TIMER_ID_0,1000,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	i2cflash_driver_handle = i2c_24xx_register(&i2c_24xx_Drv);
	for(i=0;i<I2CMEM_BUFFERSIZE;i++)
		i2cBufw[i] = i & 0xff;
	while(1)
	{
		wait_event(EVENT_TIMER | EVENT_QSPI_IRQ);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			extflash_read(i2cflash_driver_handle,0,i2cBufr,I2C_24XX_PAGESIZE*2);
			extflash_write(i2cflash_driver_handle,0,i2cBufw,I2C_24XX_PAGESIZE*2);
		}
	}
}
#endif // #ifdef AOS_STM32H7xx_HAL_I2C_H
#endif // #ifdef SAMPLE_PROCESSES_ENABLED


