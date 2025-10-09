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
 * int_dac_driver.c
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_DAC_ENABLED

#include "int_dac_driver.h"

extern	ANALOG_DriverStruct_t			ANALOG_DriverStruct[MAX_ANALOG_DRIVERS];
extern	uint8_t							last_analog_used_handle,analog_driver_request;

ITCM_AREA_CODE  static uint32_t int_dac_start(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	HAL_TIM_Base_Start_IT(timer);
	dac_drv->status |= DAC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_dac_stop(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	HAL_TIM_Base_Stop_IT(timer);
	dac_drv->status &= ~DAC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_dac_init(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
	dac_drv->status = 0;
	HAL_DAC_Start_DMA(dac_drv->dac, dac_drv->channel, (uint32_t *)dac_drv->dac_buffer, dac_drv->len,dac_drv->alignment);
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_dac_get_status(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
	return dac_drv->status;
	return 0;
}
float			fdiv;
uint32_t		pkt_out=0;
extern uint32_t	usb_pkt;

ITCM_AREA_CODE  static uint32_t int_dac_timer_set(DAC_Drv_TypeDef *dac_drv,float frequency)
{
TIM_HandleTypeDef 	*dac_timer = dac_drv->dac_timer;

	dac_drv->dac_sample_frequency = frequency;
	fdiv = ((float )HSI_CLOCK / 2.0F / frequency) - 1.0F;
	__HAL_TIM_DISABLE(dac_timer);
	dac_timer->Instance->CNT = 0;
	dac_drv->PSC = dac_timer->Instance->PSC = 0;
	dac_drv->ARR = dac_timer->Instance->ARR = (uint32_t )fdiv;
	__HAL_TIM_ENABLE(dac_timer);
	return 0;
}

ITCM_AREA_CODE  static uint32_t int_dac_play_wav(uint8_t handle,uint16_t *wav_ptr)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
Wav_Header_TypeDef  *Wav = (Wav_Header_TypeDef *)wav_ptr;
	if ((Wav->FileTypeBlocID[0] == 'R') && (Wav->FileTypeBlocID[1] == 'I')&&(Wav->FileTypeBlocID[2] == 'F') &&(Wav->FileTypeBlocID[3] == 'F'))
	{
		dac_drv->wav_ptr = &Wav->first_audio_sample;
		dac_drv->wav_len = Wav->DataSize;
		dac_drv->wav_samples_counter = 0;
		int_dac_timer_set(dac_drv,Wav->Frequency);//
		dac_drv->wav_volume = 1.0F;
		dac_drv->dac_wav_flags |= DAC_WAV_FLAGS_DO_PLAY;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE  static uint32_t int_dac_stop_wav(uint8_t handle)
{
DAC_Drv_TypeDef		*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
	dac_drv->dac_wav_flags &= ~DAC_WAV_FLAGS_DO_PLAY;
	int_dac_timer_set(dac_drv,DEFAULT_SAMPLE_FREQUENCY);
	return 0;
}

ITCM_AREA_CODE uint32_t	int_dac_register(DAC_Drv_TypeDef *private_data)
{
DAC_Drv_TypeDef	*dac_drv;
	if ( ANALOG_DriverStruct[last_analog_used_handle].process == 0 )
	{
		ANALOG_DriverStruct[last_analog_used_handle].process = get_current_process();
		ANALOG_DriverStruct[last_analog_used_handle].private_data = (uint32_t *)private_data;

		dac_drv = (DAC_Drv_TypeDef *)ANALOG_DriverStruct[last_analog_used_handle].private_data;
		if ( dac_drv->dac == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( dac_drv->dac_timer == NULL)
			return DRIVER_REQUEST_FAILED;
		if ( dac_drv->dac_sample_frequency == 0)
			dac_drv->dac_sample_frequency = DEFAULT_SAMPLE_FREQUENCY;
		else
			dac_drv->dac_sample_frequency /= 1000.0F;

		if ( dac_drv->flags == DAC_FLAGS_USE_USBMODULE)
		{
			if ( dac_drv->usbaudio_buffer == NULL)
				return DRIVER_REQUEST_FAILED;
			if ( dac_drv->usbaudio_size == 0)
				return DRIVER_REQUEST_FAILED;
		}

		int_dac_timer_set(dac_drv,dac_drv->dac_sample_frequency);

		ANALOG_DriverStruct[last_analog_used_handle].status = DRIVER_STATUS_IN_USE;
		ANALOG_DriverStruct[last_analog_used_handle].dac_start = int_dac_start;
		ANALOG_DriverStruct[last_analog_used_handle].dac_stop = int_dac_stop;
		ANALOG_DriverStruct[last_analog_used_handle].dac_get_status = int_dac_get_status;
		ANALOG_DriverStruct[last_analog_used_handle].dac_init = int_dac_init;
		ANALOG_DriverStruct[last_analog_used_handle].dac_play_wav = int_dac_play_wav;
		ANALOG_DriverStruct[last_analog_used_handle].dac_stop_wav = int_dac_stop_wav;
		last_analog_used_handle++;
		analog_driver_request++;
		return last_analog_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

/************ Interrupt *************/

ITCM_AREA_CODE  static uint32_t get_handle_from_dac_dma_channel(DAC_HandleTypeDef *hdac)
{
uint32_t	i,drv_ret=255;
	for(i=0;i<MAX_ANALOG_DRIVERS;i++)
	{
		if (( ANALOG_DriverStruct[i].status & DRIVER_STATUS_IN_USE) ==  DRIVER_STATUS_IN_USE)
		{
			if ( ANALOG_DriverStruct[i].private_data != NULL )
			{
				DAC_Drv_TypeDef	*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[i].private_data;
				if ( dac_drv->dac == hdac )
					return i;
			}
		}
	}
	return drv_ret;
}

#define DAC_WAV_ADAPTWND_SAMPLES_NUM	4096
uint32_t	dac_time_usec;
uint32_t	dac_samples=0;
uint32_t limit_hi;
uint32_t limit_lo;
ITCM_AREA_CODE  static void dac_irq_common(DAC_Drv_TypeDef	*dac_drv,uint32_t handle)
{
uint32_t	i , start_sample;
	dac_time_usec = A_get_timelapse_end();
	A_get_timelapse_start();

	start_sample = (dac_drv->status & DAC_STATUS_HALF) ? 0 : dac_drv->len/2;
	if (( dac_drv->dac_wav_flags & DAC_WAV_FLAGS_DO_PLAY) == DAC_WAV_FLAGS_DO_PLAY)
	{
		for(i=0;i<dac_drv->len/2;i++)
		{
			dac_drv->dac_buffer[i+start_sample] = (int16_t )((float )(dac_drv->wav_ptr[i] + 0x8000) * dac_drv->wav_volume);
		}
		dac_drv->wav_ptr += dac_drv->len/2;
		dac_drv->wav_samples_counter += dac_drv->len;
		if ( dac_drv->wav_samples_counter >= dac_drv->wav_len)
		{
			int_dac_stop_wav(handle);
		}
	}
	else
	{
#ifdef SOUND_ENABLED
		if (( dac_drv->flags & DAC_FLAGS_USE_SYNTHMODULE) == DAC_FLAGS_USE_SYNTHMODULE)
		{
			Do_synth( start_sample);
		}
#ifdef	USB_AUDIO
		if (( dac_drv->flags & DAC_FLAGS_USE_USBMODULE) == DAC_FLAGS_USE_USBMODULE)
		{
			__disable_irq();
			limit_hi = dac_drv->len/2 + dac_drv->len/4;
			limit_lo = dac_drv->len/4;
			if ( usb_pkt > limit_hi)
			{
				int_dac_timer_set(dac_drv,dac_drv->dac_sample_frequency-1.0F);
			}
			if ( usb_pkt < limit_lo)
			{
				int_dac_timer_set(dac_drv,dac_drv->dac_sample_frequency+1.0F);
			}
	    	for(i=0;i<dac_drv->len/2;i++)
			{
				dac_drv->dac_buffer[i+start_sample] = dac_drv->usbaudio_buffer[dac_drv->usbaudio_index] + 32768;
				dac_drv->usbaudio_index++;
				if ( dac_drv->usbaudio_index >= dac_drv->usbaudio_size)
					dac_drv->usbaudio_index= 0;
			}
			if ( start_sample == 0)
				TransferComplete_CallBack_FS();
			else
				HalfTransfer_CallBack_FS();
			dac_samples += dac_drv->len/2;
			__enable_irq();
		}
#endif // #ifdef	USB_AUDIO
#endif // #ifdef SOUND_ENABLED

	}
}

ITCM_AREA_CODE void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
uint32_t handle;
	__disable_irq();
	if ( (handle = get_handle_from_dac_dma_channel(hdac)) != 255 )
	{
		DAC_Drv_TypeDef	*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
		dac_drv->status |= DAC_STATUS_HALF;
		dac_drv->status &= ~DAC_STATUS_FULL;
		dac_irq_common(dac_drv,handle);
		if ( dac_drv->flags & DAC_FLAGS_WAKEUP)
			activate_process(ANALOG_DriverStruct[handle].process,EVENT_DAC_IRQ,HW_DAC);
	}
	__enable_irq();
}

ITCM_AREA_CODE void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
uint32_t handle;
	__disable_irq();
	if ( (handle = get_handle_from_dac_dma_channel(hdac)) != 255 )
	{
		DAC_Drv_TypeDef	*dac_drv = (DAC_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
		dac_drv->status |= DAC_STATUS_FULL;
		dac_drv->status &= ~DAC_STATUS_HALF;
		dac_irq_common(dac_drv,handle);
		if ( dac_drv->flags & DAC_FLAGS_WAKEUP)
			activate_process(ANALOG_DriverStruct[handle].process,EVENT_DAC_IRQ,HW_DAC);
	}
	__enable_irq();
}

#endif // #ifdef A_OS_DAC_ENABLED

