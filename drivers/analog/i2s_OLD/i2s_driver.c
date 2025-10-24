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
 * i2s_driver.c
 *
 *  Created on: Oct 23, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SOUND_ENGINE_I2S_ENABLED

#include "i2s_driver.h"

I2S_DriverStruct_t	*i2s = NULL;

ITCM_AREA_CODE uint32_t	i2s_driver_start(I2S_DriverStruct_t *i2s_driver)
{
	return HAL_I2SEx_TransmitReceive_DMA(i2s_driver->i2s,(uint16_t*)&i2s_driver->out_buffer[0][0],(uint16_t*)&i2s_driver->in_buffer[0][0],2 * I2S_BUFFER_SIZE);
}

ITCM_AREA_CODE uint32_t	i2s_driver_register(I2S_DriverStruct_t *i2s_driver)
{
	if (( i2s_driver->i2s == NULL ) || ( i2s_driver->out_buffer[0] == NULL ) || ( i2s_driver->out_buffer[1] == NULL ) || ( i2s_driver->in_buffer[0] == NULL ) || ( i2s_driver->in_buffer[1] == NULL ))
		return 1;
	i2s = i2s_driver;
	return 0;
}


ITCM_AREA_CODE void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if ( hi2s == i2s->i2s)
	    memcpy(i2s->out_buffer[0], i2s->in_buffer[0], 2 * I2S_BUFFER_SIZE);
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	if ( hi2s == i2s->i2s)
	    memcpy(i2s->out_buffer[1], i2s->in_buffer[1], 2 * I2S_BUFFER_SIZE);
}
#endif //#ifdef SOUND_ENGINE_I2S_ENABLED
