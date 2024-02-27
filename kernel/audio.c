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
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#include "main.h"
#include "A.h"
#include "system_default.h"
#include "A_exported_functions.h"
#include "kernel_opt.h"

#ifdef CODEC_ENABLED
#ifdef CODEC_NAU88C22
#include "audio.h"
#include "../modules/audio/effects.h"

DMA_NOCACHE_RAM	WaveLR_t	*audio_out, *audio_in;

OSCILLATORS_RAM	__attribute__ ((aligned (16))) AudioFlagsTypeDef	AudioFlags;

extern int16_t	oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];

OSCILLATORS_RAM	uint16_t	pipe[MAX_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];
OSCILLATORS_RAM	int16_t		pipe0[HALF_NUMBER_OF_AUDIO_SAMPLES];

extern	ControlAdcDef	ControlAdc;

#define	OSCILLATORS	1

uint8_t StartAudioBuffers(int16_t *audio_in_buffer,int16_t *audio_out_buffer)
{
#ifdef CODEC_NAU88C22
	if ( allocate_hw(HW_I2S2,0) == HW_I2S2)
	{
		if ( HAL_I2SEx_TransmitReceive_DMA(&NAU88C22_I2S, (uint16_t*)audio_out_buffer, (uint16_t*)audio_in_buffer,AUDIO_BUF_SIZE*2) != HAL_OK)
		{
			deallocate_hw(HW_I2S2);
			return 1;
		}
		audio_out = (WaveLR_t *)audio_out_buffer;
		audio_in  = (WaveLR_t *)audio_in_buffer;
	}
	return 0;
#endif
	return 1;
}

ITCM_AREA_CODE void get_limits(uint16_t *start,uint16_t *end)
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

extern	void do_fft(int16_t *inputData, int16_t *outputData);
uint16_t	pipe_used;
ITCM_AREA_CODE void IrqProcessSamples(void)
{
uint16_t	start,end,i;

//#define	TEST_OSCILLATORS
//	if ((AudioFlags.audio_flags & AUDIO_GENERATE_FLAG ) == AUDIO_GENERATE_FLAG)
	{
		HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);

		RunOscillator32();
		get_limits(&start,&end);
//		Do_iir(oscout_buffer,pipe0);
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			pipe0[i] = Do_Biquad_s(oscout_buffer[i]);
			audio_out[i+start].channel[AUDIO_LEFT_CH]  = pipe0[i];
			audio_out[i+start].channel[AUDIO_RIGHT_CH] = pipe0[i];
		}
	}
    HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	AudioFlags.audio_flags |= (AUDIO_HALFBUFOUT_FLAG | AUDIO_OUT_READY_FLAG);
	AudioFlags.audio_flags |= (AUDIO_HALFBUFIN_FLAG | AUDIO_IN_READY_FLAG);
	IrqProcessSamples();
}

ITCM_AREA_CODE void HAL_I2SEx_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	AudioFlags.audio_flags &= ~AUDIO_HALFBUFOUT_FLAG;
	AudioFlags.audio_flags |= AUDIO_OUT_READY_FLAG;
	AudioFlags.audio_flags &= ~AUDIO_HALFBUFIN_FLAG;
	AudioFlags.audio_flags |= AUDIO_IN_READY_FLAG;
	IrqProcessSamples();
}

void SetEffectMode(void)
{
	AudioFlags.audio_flags &= ~AUDIO_GENERATE_FLAG;
}

void SetGeneratorMode(void)
{
	AudioFlags.audio_flags |= AUDIO_GENERATE_FLAG;
}

void SetMasterVolume(uint16_t volume)
{
	if ( volume < 100 )
		AudioFlags.master_volume = (float )volume / 100.0F;
	else
		AudioFlags.master_volume = 1.0F;
}

#endif // #ifdef CODEC_NAU88C22
#endif // #ifdef CODEC_ENABLED

