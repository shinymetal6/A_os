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
 *  Created on: Oct 24, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SOUND_ENGINE_I2S_ENABLED
#include "i2s_driver.h"
#include <strings.h>

uint32_t	*first_i2s = NULL;

ITCM_AREA_CODE uint32_t	i2s_driver_start(I2S_DriverStruct_t *i2s)
{
	if (i2s == NULL)
		return 1;
	return HAL_I2SEx_TransmitReceive_DMA(i2s->i2s,i2s->i2s_tx_buffer,i2s->i2s_rx_buffer,I2S_BUFFER_SIZE);
}

ITCM_AREA_CODE uint32_t	i2s_driver_register(I2S_DriverStruct_t *i2s)
{
	if (i2s == NULL)
		return 1;
	if ( (i2s->i2s_rx_buffer == NULL) || (i2s->i2s_tx_buffer == NULL))
		return 1;
	if ( (i2s->left_rx_buffer == NULL) || (i2s->right_rx_buffer == NULL) || (i2s->left_tx_buffer == NULL) || (i2s->right_tx_buffer == NULL))
		return 1;
	if (i2s->i2s == NULL)
		return 1;
	if ( first_i2s == NULL )
	{
		i2s->pre_drv = (uint32_t *)&first_i2s;
		i2s->next_drv = NULL;
		first_i2s = (uint32_t *)i2s;
	}
	else
	{
		I2S_DriverStruct_t	*current_i2s = (I2S_DriverStruct_t	*)first_i2s;
		while(current_i2s->next_drv != NULL)
			current_i2s = (I2S_DriverStruct_t *)current_i2s->next_drv;
		current_i2s->next_drv = (uint32_t *)i2s;
		i2s->pre_drv = (uint32_t *)current_i2s;
		i2s->next_drv = NULL;
	}
	return 0;
}

ITCM_AREA_CODE static void Process_Audio(I2S_DriverStruct_t *i2s , uint16_t start_sample)
{
uint32_t i,j;

	for (i = 4,j=0; i < I2S_HALF_BUFFER_SIZE; i+=8,j++)
	{
		i2s->left_rx_buffer[j]   = i2s->i2s_rx_buffer[i+start_sample+I2S_LEFT_CHANNEL];
		i2s->right_rx_buffer[j]  = i2s->i2s_rx_buffer[i+start_sample+I2S_RIGHT_CHANNEL];
	}
	if (( i2s->flags & I2S_FLAGS_ECHO ) != I2S_FLAGS_ECHO)
		Do_Audio(start_sample);
	else
	{
		for (i = 4,j=0; i < I2S_HALF_BUFFER_SIZE; i+=8,j++)
		{
			i2s->i2s_tx_buffer[i+start_sample+I2S_LEFT_CHANNEL]   = i2s->left_rx_buffer[j];
			i2s->i2s_tx_buffer[i+start_sample+I2S_RIGHT_CHANNEL]  = i2s->right_rx_buffer[j];
		}
	}
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
I2S_DriverStruct_t *i2s = (I2S_DriverStruct_t *)first_i2s;
	if ( i2s != NULL )
	{
		while((i2s != NULL) && (i2s->i2s != hi2s))
			i2s = (I2S_DriverStruct_t *)i2s->next_drv;
		if (i2s != NULL)
			Process_Audio(i2s,I2S_SECOND_HALF);
	}
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
I2S_DriverStruct_t *i2s = (I2S_DriverStruct_t *)first_i2s;
	if ( i2s != NULL )
	{
		while((i2s != NULL) && (i2s->i2s != hi2s))
			i2s = (I2S_DriverStruct_t *)i2s->next_drv;
		if (i2s != NULL)
			Process_Audio(i2s,I2S_FIRST_HALF);
	}
}


#endif //#ifdef SOUND_ENGINE_I2S_ENABLED
