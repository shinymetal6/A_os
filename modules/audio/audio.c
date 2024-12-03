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
 * audio.c
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/kernel_opt.h"

#include "audio.h"
#include "effects.h"

#ifdef AUDIO_GENERATORS_ENABLED

DMA_NOCACHE_RAM	WaveLR_t			*audio_out, *audio_in;
AUDIO_FAST_RAM	int16_t				pipe[MAX_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];
AUDIO_FAST_RAM	int16_t				pipe_out[HALF_NUMBER_OF_AUDIO_SAMPLES];

extern int16_t	oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];

extern	ANALOG_DriverStruct_t			ANALOG_DriverStruct[MAX_ANALOG_DRIVERS];

uint8_t StartAudioBuffers(uint8_t handle,int16_t *audio_in_buffer,int16_t *audio_out_buffer)
{

#if defined(STM32H7xx_HAL_I2S_H) || defined(STM32H7xx_HAL_DAC_H)

#ifdef STM32H7xx_HAL_I2S_H
	if ( HAL_I2SEx_TransmitReceive_DMA(&hi2s2, (uint16_t*)audio_out_buffer, (uint16_t*)audio_in_buffer,AUDIO_BUF_SIZE*2) != HAL_OK)
		return 1;
	audio_out = (WaveLR_t *)audio_out_buffer;
	audio_in  = (WaveLR_t *)audio_in_buffer;
#else
#ifdef STM32H7xx_HAL_DAC_H
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].analog_driver_private_data;
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	dac_drv->status = 0;
	if ( HAL_DAC_Start_DMA(dac_drv->dac, dac_drv->channel, (uint32_t *)dac_drv->dac_buffer, dac_drv->len,dac_drv->alignment)!= HAL_OK )
		return 1;
	if ( HAL_TIM_Base_Start(timer) == 0)
		return 0;
	return 1;
#endif
#endif
	return 0;
#else
	return 1;
#endif
}

#endif // #ifdef AUDIO_GENERATORS_ENABLED



