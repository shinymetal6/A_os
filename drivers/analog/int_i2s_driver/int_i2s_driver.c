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
 * int_i2s_driver.c
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef STM32H7xx_HAL_I2S_H

#include "int_i2s_driver.h"
#include "../../../modules/sound_engine/sound_engine.h"

#ifdef SOUND_ENGINE_I2S_ENABLED
extern	ANALOG_DriverStruct_t	ANALOG_DriverStruct[MAX_ANALOG_DRIVERS];
extern	uint8_t					last_analog_used_handle,analog_driver_request;

//extern int16_t					oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];


ITCM_AREA_CODE  static uint32_t int_i2s_init(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_i2s_start(uint8_t handle)
{
I2S_Drv_TypeDef		*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
	return HAL_I2SEx_TransmitReceive_DMA(i2s_drv->i2s, (uint16_t*)i2s_drv->i2s_out_buffer, (uint16_t*)i2s_drv->i2s_in_buffer,i2s_drv->len);
}

ITCM_AREA_CODE  static uint32_t int_i2s_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_i2s_get_status(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE int16_t	*get_i2s_out_buf(uint8_t handle)
{
I2S_Drv_TypeDef		*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
	if ( i2s_drv != NULL )
		return i2s_drv->i2s_out_buffer;
	else
		return (int16_t *)NULL;
}

ITCM_AREA_CODE int16_t	*get_i2s_in_buf(uint8_t handle)
{
I2S_Drv_TypeDef		*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
	return i2s_drv->i2s_in_buffer;
}

ITCM_AREA_CODE uint32_t	int_i2s_register(I2S_Drv_TypeDef *private_data)
{
I2S_Drv_TypeDef	*i2s_drv;
	if ( ANALOG_DriverStruct[last_analog_used_handle].process == 0 )
	{
		ANALOG_DriverStruct[last_analog_used_handle].process = get_current_process();
		ANALOG_DriverStruct[last_analog_used_handle].private_data = (uint32_t *)private_data;

		i2s_drv = (I2S_Drv_TypeDef *)ANALOG_DriverStruct[last_analog_used_handle].private_data;

		if ( ( i2s_drv->i2s == NULL ) || ( i2s_drv->i2s_in_buffer == NULL ) || ( i2s_drv->i2s_out_buffer == NULL ))
				return DRIVER_REQUEST_FAILED;
		if ( i2s_drv->wakeup_id == 0 )
			return DRIVER_REQUEST_FAILED;

		ANALOG_DriverStruct[last_analog_used_handle].status = DRIVER_STATUS_IN_USE;
		ANALOG_DriverStruct[last_analog_used_handle].i2s_start = int_i2s_start;
		ANALOG_DriverStruct[last_analog_used_handle].i2s_stop = int_i2s_stop;
		ANALOG_DriverStruct[last_analog_used_handle].i2s_get_status = int_i2s_get_status;
		ANALOG_DriverStruct[last_analog_used_handle].i2s_init = int_i2s_init;
		last_analog_used_handle++;
		analog_driver_request++;
		return last_analog_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

/******** Interrupts ***/


ITCM_AREA_CODE static uint32_t get_handle_from_i2s_dma_channel(I2S_HandleTypeDef *hi2s)
{
uint32_t	i,drv_ret=255;
	for(i=0;i<MAX_ANALOG_DRIVERS;i++)
	{
		if (( ANALOG_DriverStruct[i].status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
		{
			if ( ANALOG_DriverStruct[i].private_data != NULL )
			{
				I2S_Drv_TypeDef	*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[i].private_data;
				if ( i2s_drv->i2s == hi2s )
					return i;
			}
		}
	}
	return drv_ret;
}

ITCM_AREA_CODE  void i2s_irq_common(I2S_Drv_TypeDef	*i2s_drv,uint32_t handle)
{
uint32_t	start_sample;
#ifdef TOUCH_CS_GPIO_Port
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
#endif //#ifdef TOUCH_CS_GPIO_Port
	start_sample = (i2s_drv->status & I2S_STATUS_HALF) ? 0 : i2s_drv->len/2;
	if (( i2s_drv->flags & I2S_FLAGS_USE_SYNTHMODULE) == I2S_FLAGS_USE_SYNTHMODULE)
		Do_Audio(start_sample);
#ifdef TOUCH_CS_GPIO_Port
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
#endif //#ifdef TOUCH_CS_GPIO_Port
}

extern	int16_t		i2s_in_buffer[2][512];
extern	int16_t		i2s_out_buffer[2][512];
void ProcessAudioHalf(void)
{
    // Example: echo RX to TX
    memcpy(i2s_out_buffer[0], i2s_in_buffer[0], sizeof(i2s_out_buffer[0]));
}


void ProcessAudioFull(void)
{
    // Example: echo RX to TX
    memcpy(i2s_out_buffer[1], i2s_in_buffer[1], sizeof(i2s_out_buffer[1]));
}


ITCM_AREA_CODE void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	/*
uint32_t handle;
	if ( (handle = get_handle_from_i2s_dma_channel(hi2s)) != 255 )
	{
		I2S_Drv_TypeDef	*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
		i2s_drv->status |= (I2S_STATUS_HALF | I2S_STATUS_DATA_READY);
		i2s_drv->status &= ~I2S_STATUS_FULL;
		i2s_irq_common(i2s_drv,handle);
	}
	*/
	ProcessAudioHalf();
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	/*
uint32_t handle;
	if ( (handle = get_handle_from_i2s_dma_channel(hi2s)) != 255 )
	{
		I2S_Drv_TypeDef	*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
		i2s_drv->status |= (I2S_STATUS_FULL | I2S_STATUS_DATA_READY);
		i2s_drv->status &= ~I2S_STATUS_HALF;
		i2s_irq_common(i2s_drv,handle);
	}
	*/
	ProcessAudioFull();
}
#endif // #ifdef SOUND_ENGINE_I2S_ENABLED

#endif // #ifdef STM32H7xx_HAL_I2S_H
