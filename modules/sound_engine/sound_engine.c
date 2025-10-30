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
 * sound_engine.c
 *
 *  Created on: Oct 20, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

float	Sound_Sample_Frequency = DEFAULT_SAMPLE_FREQUENCY;
#ifdef SOUND_ENGINE_ENABLED
#include "sound_engine.h"

uint8_t	num_effects=0;

AUDIO_Source_TypeDef *AudioSource[2] = {NULL,NULL};

ITCM_AREA_CODE PTR_Effect_TypeDef *Sound_Apply_Effect(uint32_t *effect)
{
uint8_t		done=0;
PTR_Effect_TypeDef	*PTR_Effect = (PTR_Effect_TypeDef *)effect;
	num_effects=0;
	while( done < 8 )
	{
		if ( (PTR_Effect->status & SOUND_EFFECT_INITIALIZED) == SOUND_EFFECT_INITIALIZED)
			PTR_Effect->effect( (uint32_t *)PTR_Effect);
		if ( PTR_Effect->next_effect != NULL )
			PTR_Effect = (PTR_Effect_TypeDef *)PTR_Effect->next_effect;
		else
			return PTR_Effect;
		num_effects++;
		done++;
	}
	return NULL;
}

ITCM_AREA_CODE uint8_t Sound_Insert_Effect(uint32_t *ext_source,uint32_t *new_effect)
{
PTR_Effect_TypeDef		*effect = (PTR_Effect_TypeDef *)new_effect , *pre_effect;
AUDIO_Source_TypeDef	*sound_source = (AUDIO_Source_TypeDef *)ext_source;

	if (( ext_source == NULL ) || ( new_effect == NULL ))
			return 1;
	if ( sound_source->next_effect == NULL )
	{
		sound_source->next_effect = (uint32_t *)effect;
		effect->next_effect = NULL;
		effect->in_buf = sound_source->in_buf;
		sound_source->out_buf = effect->out_buf;
		effect->device_out_buf = sound_source->device_out_buf;
		sound_source->out_buf = effect->in_buf;
	}
	else
	{
		pre_effect = (PTR_Effect_TypeDef *)sound_source;
		while ( sound_source->next_effect != NULL)
		{
			pre_effect = (PTR_Effect_TypeDef *)sound_source->next_effect;
			sound_source = (AUDIO_Source_TypeDef *)sound_source->next_effect;
		}
		sound_source->next_effect = (uint32_t *)effect;
		effect->next_effect = NULL;
		effect->in_buf = pre_effect->out_buf;
		effect->device_out_buf = sound_source->device_out_buf;
	}
	effect->in_device = sound_source->in_device;
	effect->out_device = sound_source->out_device;
	effect->channel_in = sound_source->channel_in;
	effect->channel_out = sound_source->channel_out;
	effect->block_size = sound_source->block_size;
	if ( effect->effect_init != NULL )
	{
		effect->effect_init(new_effect);
		effect->status |= SOUND_EFFECT_INITIALIZED;
	}
	return 0;
}

ITCM_AREA_CODE uint8_t Sound_Remove_Effect(uint32_t *ext_source,uint32_t *remove_effect)
{
PTR_Effect_TypeDef		*effect = (PTR_Effect_TypeDef *)remove_effect , *pre_effect;
AUDIO_Source_TypeDef	*sound_source = (AUDIO_Source_TypeDef *)ext_source;
	return 0;
}

ITCM_AREA_CODE uint8_t Sound_Change_Sample_Frequency(uint32_t new_sample_frequency)
{
	if (( new_sample_frequency >= 8000) && ( new_sample_frequency <= 96000))
	{
		Sound_Sample_Frequency = (float )new_sample_frequency;
		return 0;
	}
	return 1;
}

uint8_t number_of_synths = 0;

#ifdef SOUND_ENGINE_I2S_ENABLED
ITCM_AREA_CODE void to_i2sout(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel)
{
uint32_t i,j;
	for (i = 4,j=0; i < I2S_HALF_BUFFER_SIZE; i+=8,j++)
		audio_out[i + start_sample+channel] = audio_in[j];
}
#else
ITCM_AREA_CODE void to_dacout(int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples,uint8_t channel)
{
uint32_t i;
	for (i = 0; i < num_samples; i++)
		audio_out[i + start_sample] = (audio_in[i] + 32768) >> 4;
//		audio_out[i + start_sample] = audio_in[i] >> 4;
}
#endif // #ifdef SOUND_ENGINE_I2S_ENABLED

ITCM_AREA_CODE void Do_Audio(uint32_t start_sample)
{
PTR_Effect_TypeDef *last_effect;
uint8_t i=0;

	for(i=0;i<SOUND_AUDIO_SOURCES;i++)
	{
		if ( AudioSource[i] != NULL)
		{
			AUDIO_Source_TypeDef *source = AudioSource[i];
			if ( source->status == SOURCE_ENABLED )
			{
				if ( AudioSource[i]->source_type == SOUND_SOURCE_IS_SYNTH)
					Synth_Process_Block((uint32_t *)source);
				last_effect = (PTR_Effect_TypeDef *)source;
				if ( source->next_effect != NULL )
				{
					last_effect = (PTR_Effect_TypeDef *)Sound_Apply_Effect(source->next_effect);
					source->OutFunc(last_effect->device_out_buf,last_effect->out_buf,start_sample,source->block_size,source->channel_out);
				}
				else
					source->OutFunc(source->device_out_buf,source->out_buf,start_sample,source->block_size,source->channel_out);
			}
		}
	}
}

#endif // #ifdef SOUND_ENGINE_ENABLED

