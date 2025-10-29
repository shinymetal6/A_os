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
 * dac_driver.c
 *
 *  Created on: Oct 29, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_DAC_ENABLED

#include "dac_driver.h"
uint32_t	*first_dac = NULL;

ITCM_AREA_CODE  uint32_t dac_start(DAC_DriverStruct_t *dac_drv)
{
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	HAL_TIM_Base_Start_IT(timer);
	dac_drv->status |= DAC_STATUS_RUNNING;
	return 0;
}

ITCM_AREA_CODE  uint32_t dac_stop(DAC_DriverStruct_t *dac_drv)
{
TIM_HandleTypeDef	*timer = dac_drv->dac_timer;
	HAL_TIM_Base_Stop_IT(timer);
	dac_drv->status &= ~DAC_STATUS_RUNNING;
	return 0;
}

/* forward declaration */
extern   uint32_t dac_timer_set(DAC_DriverStruct_t *dac_drv,float frequency);

ITCM_AREA_CODE  uint32_t dac_play_wav(DAC_DriverStruct_t *dac_drv,uint16_t *wav_ptr)
{
Wav_Header_TypeDef  *Wav = (Wav_Header_TypeDef *)wav_ptr;
	if ((Wav->FileTypeBlocID[0] == 'R') && (Wav->FileTypeBlocID[1] == 'I')&&(Wav->FileTypeBlocID[2] == 'F') &&(Wav->FileTypeBlocID[3] == 'F'))
	{
		dac_drv->wav_ptr = &Wav->first_audio_sample;
		dac_drv->wav_len = Wav->DataSize;
		dac_drv->wav_samples_counter = 0;
		dac_timer_set(dac_drv,Wav->Frequency);//
		dac_drv->wav_volume = 1.0F;
		dac_drv->dac_wav_flags |= DAC_WAV_FLAGS_DO_PLAY;
		return 0;
	}
	return 1;
}

ITCM_AREA_CODE  uint32_t dac_stop_wav(DAC_DriverStruct_t *dac_drv)
{
	dac_drv->dac_wav_flags &= ~DAC_WAV_FLAGS_DO_PLAY;
	dac_timer_set(dac_drv,DEFAULT_SAMPLE_FREQUENCY);
	return 0;
}
ITCM_AREA_CODE  uint32_t dac_init(DAC_DriverStruct_t *dac_drv)
{
	dac_drv->status = HAL_DAC_Start_DMA(dac_drv->dac, dac_drv->channel, (uint32_t *)dac_drv->dac_buffer, dac_drv->len,dac_drv->alignment);
	return dac_drv->status;
}

ITCM_AREA_CODE  uint32_t dac_get_status(DAC_DriverStruct_t *dac_drv)
{
	return dac_drv->status;
	return 0;
}

float			fdiv;
uint32_t		pkt_out=0;
extern uint32_t	usb_pkt;

ITCM_AREA_CODE   uint32_t dac_timer_set(DAC_DriverStruct_t *dac_drv,float frequency)
{
TIM_HandleTypeDef 	*dac_timer = dac_drv->dac_timer;

	dac_drv->dac_sample_frequency = frequency;
	//fdiv = ((float )HSI_CLOCK / 2.0F / frequency) - 1.0F;
	fdiv = ((float )HSI_CLOCK / 1.0F / frequency) - 1.0F;
	__HAL_TIM_DISABLE(dac_timer);
	dac_timer->Instance->CNT = 0;
	dac_drv->PSC = dac_timer->Instance->PSC = 0;
	dac_drv->ARR = dac_timer->Instance->ARR = (uint32_t )fdiv;
	__HAL_TIM_ENABLE(dac_timer);
	return 0;
}
ITCM_AREA_CODE uint32_t	dac_register(DAC_DriverStruct_t *dac)
{
	if (dac == NULL)
		return 1;
	if (dac->dac_buffer == NULL)
		return 1;
	if ( (dac->dac == NULL) || (dac->dac_timer == NULL))
		return 1;

	if ( first_dac == NULL )
	{
		dac->pre_drv = (uint32_t *)&first_dac;
		dac->next_drv = NULL;
		first_dac = (uint32_t *)dac;
	}
	else
	{
		DAC_DriverStruct_t	*current_dac = (DAC_DriverStruct_t	*)first_dac;
		while(current_dac->next_drv != NULL)
			current_dac = (DAC_DriverStruct_t *)current_dac->next_drv;
		current_dac->next_drv = (uint32_t *)dac;
		dac->pre_drv = (uint32_t *)current_dac;
		dac->next_drv = NULL;
	}
	dac->process = get_current_process();
	dac_timer_set(dac,dac->dac_sample_frequency);

	return 0;
}

/************ Interrupt *************/

#define DAC_WAV_ADAPTWND_SAMPLES_NUM	4096
uint32_t	dac_time_usec;
uint32_t	dac_samples=0;
uint32_t limit_hi;
uint32_t limit_lo;

void dac_irq_common(DAC_DriverStruct_t *dac_drv)
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
			dac_stop_wav(dac_drv);
		}
	}
	else
	{
#ifdef SOUND_ENGINE_ENABLED
		if (( dac_drv->flags & DAC_FLAGS_USE_SYNTHMODULE) == DAC_FLAGS_USE_SYNTHMODULE)
		{
			Do_Audio( start_sample);
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
#endif // #ifdef SOUND_ENGINE_ENABLED

	}
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
DAC_DriverStruct_t *dac_drv = (DAC_DriverStruct_t *)first_dac;
	if ( dac_drv != NULL )
	{
		while((dac_drv != NULL) && (dac_drv->dac != hdac))
			dac_drv = (DAC_DriverStruct_t *)dac_drv->next_drv;
		if (dac_drv != NULL)
		{
			dac_drv->status |= DAC_STATUS_HALF;
			dac_drv->status &= ~DAC_STATUS_FULL;
			dac_irq_common(dac_drv);
		}
	}
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
DAC_DriverStruct_t *dac_drv = (DAC_DriverStruct_t *)first_dac;
	if ( dac_drv != NULL )
	{
		while((dac_drv != NULL) && (dac_drv->dac != hdac))
			dac_drv = (DAC_DriverStruct_t *)dac_drv->next_drv;
		if (dac_drv != NULL)
		{
			dac_drv->status |= DAC_STATUS_FULL;
			dac_drv->status &= ~DAC_STATUS_HALF;
			dac_irq_common(dac_drv);
			if ( dac_drv->flags & DAC_FLAGS_WAKEUP)
				activate_process(dac_drv->process,EVENT_DAC_IRQ,HW_DAC);
		}
	}
}

#endif // #ifdef A_OS_DAC_ENABLED


