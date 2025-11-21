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
 * out_stage.c
 *
 *  Created on: Nov 4, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_I2S_ENABLED
#include "../sound_engine.h"
#include "out_stage.h"

ITCM_AREA_CODE void to_i2sout(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel)
{
uint32_t i,j;
	for (i = 4,j=0; i < I2S_HALF_BUFFER_SIZE; i+=8,j++)
		audio_out[i + start_sample+channel] = audio_in[j];
}
ITCM_AREA_CODE void to_dacout(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel)
{
uint32_t i;
	for (i = 0; i < num_samples; i++)
		audio_out[i + start_sample] = (audio_in[i] + 32768) >> 4;
}
ITCM_AREA_CODE void to_intbuf(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel)
{
uint32_t i;
	for (i = 0; i < num_samples; i++)
		audio_out[i + start_sample] = audio_in[i];
}



ITCM_AREA_CODE uint8_t OutStage_Register(AUDIO_Dest_TypeDef *out_stage)
{
AUDIO_Source_TypeDef *source;

	if ( out_stage->out_buf == NULL )
		return 1;
	if (( out_stage->out_device != SOURCE_TO_DAC_OUT ) && ( out_stage->out_device != SOURCE_TO_I2S_OUT ))
		return 1;

	if ( out_stage->in_buf == NULL )
	{
		source = AudioSourceLeft;
		if ( source == NULL )
			return 1;
		while(source != NULL )
		{
			if ( source->next_source == NULL )
				out_stage->in_buf = source->out_buf;
			source = (AUDIO_Source_TypeDef * )source->next_source;
		}
		if ( out_stage->in_buf == NULL )
			return 1;
	}

	if ( out_stage->channel == AUDIO_DESTINATION_LEFT)
		AudioDestLeft = out_stage;
	if ( out_stage->channel == AUDIO_DESTINATION_RIGHT)
		AudioDestRight = out_stage;
	if ( out_stage->out_device == SOURCE_TO_DAC_OUT)
		out_stage->OutFunc = to_dacout;
	else
		out_stage->OutFunc = to_i2sout;
	return 0;
}
#endif // #ifdef SOUND_ENGINE_I2S_ENABLED

