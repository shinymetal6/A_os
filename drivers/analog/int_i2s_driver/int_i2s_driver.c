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
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
#include "../../../kernel/kernel_opt.h"

#ifdef STM32H7xx_HAL_I2S_H

#include "int_i2s_driver.h"
#include "../../../modules/audio/algo/effects.h"

extern	ANALOG_DriverStruct_t	ANALOG_DriverStruct[MAX_ANALOG_DRIVERS];
extern	uint8_t					last_analog_used_handle,analog_driver_request;

extern int16_t					oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];

extern	WaveLR_t				*audio_out, *audio_in;
extern	AudioFlagsTypeDef		AudioFlags;
extern	int16_t					pipe[MAX_BLOCK_EFFECTS+MAX_SINGLESAMPLE_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];

ITCM_AREA_CODE  static uint32_t int_i2s_init(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_i2s_start(uint8_t handle)
{
I2S_Drv_TypeDef		*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
	if ( HAL_I2SEx_TransmitReceive_DMA(i2s_drv->i2s, (uint16_t*)i2s_drv->audio_out_buffer, (uint16_t*)i2s_drv->audio_in_buffer,AUDIO_BUF_SIZE*2))
		return 1;
	audio_out = (WaveLR_t *)i2s_drv->audio_out_buffer;
	audio_in  = (WaveLR_t *)i2s_drv->audio_in_buffer;
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_i2s_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_i2s_get_status(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE uint32_t	i2s_register(I2S_Drv_TypeDef *analog_driver_private_data,uint32_t driver_flags)
{
I2S_Drv_TypeDef	*i2s_drv;
	if ( ANALOG_DriverStruct[last_analog_used_handle].process == 0 )
	{
		ANALOG_DriverStruct[last_analog_used_handle].process = get_current_process();
		ANALOG_DriverStruct[last_analog_used_handle].flags |= driver_flags;
		ANALOG_DriverStruct[last_analog_used_handle].analog_driver_private_data = (uint32_t *)analog_driver_private_data;

		i2s_drv = (I2S_Drv_TypeDef *)ANALOG_DriverStruct[last_analog_used_handle].analog_driver_private_data;

		if ( ( i2s_drv->i2s == NULL ) || ( i2s_drv->audio_in_buffer == NULL ) || ( i2s_drv->audio_out_buffer == NULL ))
				return DRIVER_REQUEST_FAILED;
		ANALOG_DriverStruct[last_analog_used_handle].status = DRIVER_STATUS_REQUESTED;
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
			if ( ANALOG_DriverStruct[i].analog_driver_private_data != NULL )
			{
				I2S_Drv_TypeDef	*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[i].analog_driver_private_data;
				if ( i2s_drv->i2s == hi2s )
					return i;
			}
		}
	}
	return drv_ret;
}

ITCM_AREA_CODE static void get_limits(uint16_t *start,uint16_t *end)
{
	if (( AudioFlags.audio_flags & AUDIO_HALFBUFOUT_FLAG ) == AUDIO_HALFBUFOUT_FLAG)
	{
		*start=0;
		*end = HALF_NUMBER_OF_AUDIO_SAMPLES;
	}
	else
	{
		*start=HALF_NUMBER_OF_AUDIO_SAMPLES;
		*end = NUMBER_OF_AUDIO_SAMPLES;
	}
}

ITCM_AREA_CODE static void IrqProcessSamples(void)
{
uint16_t	start,end,i,pipe_nr;
	get_limits(&start,&end);
	if ((AudioFlags.audio_flags & AUDIO_GENERATE_FLAG ) == AUDIO_GENERATE_FLAG)
	{
		RunOscillator32();
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			pipe[0][i] = oscout_buffer[i];

		pipe_nr = BlockEffectsSequencer();
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			audio_out[i+start].channel[AUDIO_LEFT_CH]  = pipe[pipe_nr][i];
			audio_out[i+start].channel[AUDIO_RIGHT_CH] = pipe[pipe_nr][i];
		}
	}
	else
	{
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			pipe[0][i] = audio_in[i+start].channel[AUDIO_LEFT_CH];
		}
		get_limits(&start,&end);
		pipe_nr = BlockEffectsSequencer();
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			audio_out[i+start].channel[AUDIO_LEFT_CH]  = pipe[pipe_nr][i];
			audio_out[i+start].channel[AUDIO_RIGHT_CH] = pipe[pipe_nr][i];
		}
	}
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
uint32_t handle;
	if ( (handle = get_handle_from_i2s_dma_channel(hi2s)) != 255 )
	{
		I2S_Drv_TypeDef	*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
		i2s_drv->audio_flags |= (AUDIO_HALFBUFOUT_FLAG | AUDIO_OUT_READY_FLAG);
		i2s_drv->audio_flags |= (AUDIO_HALFBUFIN_FLAG  | AUDIO_IN_READY_FLAG );
	}
	IrqProcessSamples();
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
uint32_t handle;
	if ( (handle = get_handle_from_i2s_dma_channel(hi2s)) != 255 )
	{
		I2S_Drv_TypeDef	*i2s_drv = (I2S_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
		i2s_drv->audio_flags &= ~( AUDIO_HALFBUFOUT_FLAG | AUDIO_HALFBUFIN_FLAG);
		i2s_drv->audio_flags |= (AUDIO_OUT_READY_FLAG |AUDIO_IN_READY_FLAG );
	}
	IrqProcessSamples();
}

#endif // #ifdef STM32H7xx_HAL_I2S_H
