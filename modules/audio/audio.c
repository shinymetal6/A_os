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
//#include "../../kernel/kernel_opt.h"
#ifdef STM32H7xx_HAL_I2S_H


#include "audio.h"
#include "algo/effects.h"

DMA_NOCACHE_RAM	WaveLR_t	*audio_out, *audio_in;

AUDIO_FAST_RAM	__attribute__ ((aligned (16))) AudioFlagsTypeDef	AudioFlags;

extern int16_t	oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];

AUDIO_FAST_RAM	int16_t		pipe[MAX_BLOCK_EFFECTS+MAX_SINGLESAMPLE_EFFECTS] [HALF_NUMBER_OF_AUDIO_SAMPLES];

#define	OSCILLATORS	1

uint8_t StartAudioBuffers(int16_t *audio_in_buffer,int16_t *audio_out_buffer)
{
	if ( HAL_I2SEx_TransmitReceive_DMA(&hi2s2, (uint16_t*)audio_out_buffer, (uint16_t*)audio_in_buffer,AUDIO_BUF_SIZE*2) != HAL_OK)
	{
		return 1;
	}
	audio_out = (WaveLR_t *)audio_out_buffer;
	audio_in  = (WaveLR_t *)audio_in_buffer;
}
/*
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
uint16_t	start,end,i,pipe_nr;
#if defined DEBUG_FLAG_GPIO_Port
	HAL_GPIO_WritePin(DEBUG_FLAG_GPIO_Port, DEBUG_FLAG_Pin, GPIO_PIN_SET);
#endif
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
#if defined DEBUG_FLAG_GPIO_Port
    HAL_GPIO_WritePin(DEBUG_FLAG_GPIO_Port, DEBUG_FLAG_Pin, GPIO_PIN_RESET);
#endif
}
*/
/*
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
*/

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

#endif


