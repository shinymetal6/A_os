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

Synth_TypeDef *AudioSource[2] = {NULL,NULL};

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
PTR_Effect_TypeDef	*effect = (PTR_Effect_TypeDef *)new_effect , *pre_effect;
Synth_TypeDef		*synth = (Synth_TypeDef *)ext_source;
int16_t				*out_buf;
	if (( ext_source == NULL ) || ( new_effect == NULL ) || ( effect->effect_in_buf == NULL ))
			return 1;
	if ( synth->source_type == SOURCE_IS_SYNTH)
	{
		if ( synth->next_effect == NULL )
		{
			synth->next_effect = (uint32_t *)effect;
			effect->pre_effect = (uint32_t *)synth;
			effect->next_effect = NULL;
			out_buf = synth->synth_out_buf;
			synth->synth_out_buf = effect->effect_in_buf;
			effect->effect_out_buf = out_buf;
			effect->out_device = synth->out_device;
			effect->synth_block_size = synth->synth_block_size;
			synth->out_device = SOURCE_I2S_OUT;
			if ( effect->effect_init != NULL )
				effect->effect_init(new_effect);
			effect->status |= SOUND_EFFECT_INITIALIZED;
		}
		else
		{
			pre_effect = effect = (PTR_Effect_TypeDef *)synth;
			while ( effect->next_effect != NULL)
			{
				pre_effect = (PTR_Effect_TypeDef *)effect->next_effect;
				effect = (PTR_Effect_TypeDef *)effect->next_effect;
			}
			effect->next_effect = new_effect;
			effect = (PTR_Effect_TypeDef *)pre_effect->next_effect;
			effect->next_effect = NULL;
			effect->pre_effect = (uint32_t *)pre_effect;
			out_buf = pre_effect->effect_out_buf;
			pre_effect->effect_out_buf = effect->effect_in_buf;
			effect->effect_out_buf = out_buf;
			effect->out_device = pre_effect->out_device;
			effect->synth_block_size = pre_effect->synth_block_size;
			pre_effect->out_device = SOURCE_I2S_OUT;
			if ( effect->effect_init != NULL )
				effect->effect_init(new_effect);
			effect->status |= SOUND_EFFECT_INITIALIZED;
		}
	}
	return 0;
}

ITCM_AREA_CODE uint8_t Sound_Remove_Effect(uint32_t *ext_source,uint32_t *remove_effect)
{
PTR_Effect_TypeDef	*effect,*pre_effect;
Synth_TypeDef		*synth = (Synth_TypeDef *)ext_source;

	if ( synth->source_type == SOURCE_IS_SYNTH)
	{
		if ( synth->next_effect == NULL )
			return 1;

		pre_effect = effect = (PTR_Effect_TypeDef *)synth;
		while ( effect != (PTR_Effect_TypeDef *)remove_effect )
		{
			if ( effect->next_effect == NULL)
				return 1;
			pre_effect = (PTR_Effect_TypeDef *)effect->pre_effect;
			effect = (PTR_Effect_TypeDef *)effect->next_effect;
		}
		pre_effect->pre_effect = (uint32_t *)pre_effect;
		pre_effect->next_effect = effect->next_effect;
		pre_effect->effect_out_buf = effect->effect_out_buf;
	}
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

extern	Synth_TypeDef *Synth[2];

ITCM_AREA_CODE inline void synth_to_i2s_out(uint8_t channel,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples)
{
uint32_t i;
	for ( i=0;i<num_samples;i++)
		audio_out[i*2 + start_sample + channel] = audio_in[i];
}

ITCM_AREA_CODE inline void synth_to_dac_out(uint8_t channel,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples)
{
uint32_t i;
	for ( i=0;i<num_samples;i++)
		audio_out[i + start_sample] = (int16_t )((uint32_t )(audio_in[i] + 32768) >> 4);
}

ITCM_AREA_CODE inline void i2sin_to_i2sout(uint8_t channel,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample,uint16_t num_samples)
{
uint32_t i;
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
	for ( i=0;i<num_samples;i++)
		audio_out[i*2 + start_sample + channel] = audio_in[i];
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);
}



ITCM_AREA_CODE void Do_Audio(uint32_t start_sample)
{
PTR_Effect_TypeDef *last_effect;
uint8_t i=0;

	while ( AudioSource[i] != NULL)
	{
		if ( AudioSource[i]->source_type == SOURCE_IS_SYNTH)
		{
			Synth_TypeDef *synth = AudioSource[i];
			if (( synth == NULL ) || ( i == 2 ) || ( synth->status != SOURCE_ENABLED ))
				break;

			Synth_Process_Block((uint32_t *)synth,start_sample);
			if ( synth->next_effect != NULL )
			{
				last_effect = (PTR_Effect_TypeDef *)Sound_Apply_Effect(synth->next_effect);
				synth->OutFunc(i,synth->codec_buf,last_effect->effect_out_buf,start_sample,synth->synth_block_size);
			}
			else
				synth->OutFunc(i,synth->codec_buf,synth->synth_out_buf,start_sample,synth->synth_block_size);
		}
		if ( AudioSource[i]->source_type == SYNTH_IS_I2S_IN)
		{
			AudioSource_TypeDef *i2s_in = AudioSource[i];
			if (( i2s_in == NULL ) || ( i == 2 ) || ( i2s_in->status != SOURCE_ENABLED ))
				break;
			if ( i2s_in->next_effect != NULL )
			{
				last_effect = (PTR_Effect_TypeDef *)Sound_Apply_Effect(i2s_in->next_effect);
				i2s_in->OutFunc(i,i2s_in->codec_buf,last_effect->effect_out_buf,start_sample,i2s_in->synth_block_size);
			}
			else
				i2s_in->OutFunc(i,i2s_in->codec_buf,i2s_in->synth_out_buf,start_sample,i2s_in->synth_block_size);
		}

		i++;
	}
}

#endif // #ifdef SOUND_ENGINE_ENABLED

