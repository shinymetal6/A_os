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
#ifdef SOUND_ENGINE_ENABLED
#include "sound_engine.h"
uint8_t	num_effects=0;

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
PTR_Effect_TypeDef	*effect = (PTR_Effect_TypeDef *)new_effect , *last_effect;
Synth_TypeDef		*synth = (Synth_TypeDef *)ext_source;
int16_t				*out_buf;
	if (( ext_source == NULL ) || ( new_effect == NULL ) || ( effect->effect_in_buf == NULL ))
			return 1;
	if ( synth->source_type == SOUND_SOURCE_SYNTH)
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
			synth->out_device = SYNTH_I2S_OUT;
			if ( effect->effect_init != NULL )
				effect->effect_init(new_effect);
			effect->status |= SOUND_EFFECT_INITIALIZED;
		}
		else
		{
			last_effect = effect = (PTR_Effect_TypeDef *)synth->next_effect;
			while ( effect->next_effect != NULL)
			{
				last_effect = (PTR_Effect_TypeDef *)effect->next_effect;
				effect = (PTR_Effect_TypeDef *)effect->next_effect;
			}
			last_effect->next_effect = new_effect;
			effect = (PTR_Effect_TypeDef *)last_effect->next_effect;
			effect->next_effect = NULL;
			out_buf = last_effect->effect_out_buf;
			last_effect->effect_out_buf = effect->effect_in_buf;
			effect->effect_out_buf = out_buf;
			effect->out_device = last_effect->out_device;
			last_effect->out_device = SYNTH_I2S_OUT;
			if ( effect->effect_init != NULL )
				effect->effect_init(new_effect);
			effect->status |= SOUND_EFFECT_INITIALIZED;
		}
	}
	return 0;
}

uint8_t number_of_synths = 0;

ITCM_AREA_CODE inline void audio_to_i2s_out(uint8_t synth_number,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample)
{
uint32_t i;
	for ( i=0;i<SYNTH_BLOCK_SIZE;i++)
		audio_out[i*2 + start_sample*2 + synth_number] = audio_in[i+start_sample];
}

ITCM_AREA_CODE inline void audio_to_dac_out(uint8_t synth_number,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample)
{
uint32_t i;
	for ( i=0;i<SYNTH_BLOCK_SIZE;i++)
		audio_out[i + start_sample] = (int16_t )((uint32_t )(audio_in[i] + 32768) >> 4);
}

extern	Synth_TypeDef *Synth[2];

ITCM_AREA_CODE void Do_synth(uint8_t synth_number,uint32_t start_sample)
{
	PTR_Effect_TypeDef *last_effect;

	Synth_TypeDef *synth = Synth[synth_number];
	if (( synth == NULL ) || ( synth_number == number_of_synths ) || ( synth->status != SYNTH_ENABLED ))
		return;

	Synth_Process_Block((uint32_t *)synth,start_sample);
	if ( synth->next_effect != NULL )
	{
		last_effect = (PTR_Effect_TypeDef *)Sound_Apply_Effect(synth->next_effect);
		synth->OutFunc(synth_number,synth->codec_buf,last_effect->effect_out_buf,start_sample);
	}
	else
		synth->OutFunc(synth_number,synth->codec_buf,synth->synth_out_buf,start_sample);
}

#endif // #ifdef SOUND_ENGINE_ENABLED

