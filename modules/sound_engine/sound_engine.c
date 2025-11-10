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

AUDIO_Source_TypeDef	*AudioSourceLeft = NULL;
AUDIO_Source_TypeDef	*AudioSourceRight = NULL;
AUDIO_Dest_TypeDef		*AudioDestLeft  = NULL;
AUDIO_Dest_TypeDef		*AudioDestRight = NULL;
AUDIO_FAST_RAM	uint32_t			audio_pipe_time_start;
AUDIO_FAST_RAM	uint32_t			audio_pipe_time;

ITCM_AREA_CODE inline float fast_tanh(float x)
{
    if (x > 1.5f) return 1.0f;
    if (x < -1.5f) return -1.0f;
    float x2 = x * x;
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

ITCM_AREA_CODE AUDIO_Effect_TypeDef *Sound_Apply_Effect(uint32_t *effect)
{
uint8_t		done=0;
AUDIO_Effect_TypeDef	*AUDIO_Effect = (AUDIO_Effect_TypeDef *)effect;
	while( done < 32 )
	{
		if ( (AUDIO_Effect->status & SOUND_EFFECT_INITIALIZED) == SOUND_EFFECT_INITIALIZED)
			AUDIO_Effect->effect( (uint32_t *)AUDIO_Effect);
		if ( AUDIO_Effect->next_effect != NULL )
			AUDIO_Effect = (AUDIO_Effect_TypeDef *)AUDIO_Effect->next_effect;
		else
			return AUDIO_Effect;
		done++;
	}
	return NULL;
}

ITCM_AREA_CODE uint8_t Sound_Insert_Effect(uint32_t *ext_source,uint32_t *new_effect)
{
AUDIO_Effect_TypeDef	*effect = (AUDIO_Effect_TypeDef *)new_effect , *pre_effect;
AUDIO_Source_TypeDef	*sound_source = (AUDIO_Source_TypeDef *)ext_source;
AUDIO_Dest_TypeDef 		*dest = AudioDestLeft;

	if (( ext_source == NULL ) || ( new_effect == NULL ))
			return 1;
	if ( sound_source->next_effect == NULL )
	{
		sound_source->next_effect = (uint32_t *)effect;
		effect->in_buf = sound_source->out_buf;
		effect->next_effect = NULL;
	}
	else
	{
		pre_effect = (AUDIO_Effect_TypeDef *)sound_source;
		while ( sound_source->next_effect != NULL)
		{
			pre_effect = (AUDIO_Effect_TypeDef *)sound_source->next_effect;
			sound_source = (AUDIO_Source_TypeDef *)sound_source->next_effect;
		}
		sound_source->next_effect = (uint32_t *)effect;
		effect->next_effect = NULL;
		effect->in_buf = pre_effect->out_buf;
	}
	dest->in_buf = effect->out_buf;

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
AUDIO_Effect_TypeDef	*pre_effect;
AUDIO_Source_TypeDef	*sound_source = (AUDIO_Source_TypeDef *)ext_source;

	if (( ext_source == NULL ) || ( remove_effect == NULL ) || ( sound_source->next_effect == NULL ))
		return 1;

	while((sound_source != NULL) && (sound_source != (AUDIO_Source_TypeDef *)remove_effect))
	{
		pre_effect = (AUDIO_Effect_TypeDef *)sound_source;
		sound_source = (AUDIO_Source_TypeDef *)sound_source->next_effect;
	}
	if ((pre_effect == NULL) || (sound_source == NULL))
		return 1;
	pre_effect->next_effect = sound_source->next_effect;


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

ITCM_AREA_CODE static void audio_gen(AUDIO_Source_TypeDef *source,AUDIO_Dest_TypeDef *dest,uint32_t start_sample,uint8_t device)
{
AUDIO_Effect_TypeDef *last_effect;
	if ( source->source_type == SOUND_SOURCE_IS_SYNTH )
	{
		Synth_Process_Block((uint32_t *)source);
		last_effect = (AUDIO_Effect_TypeDef *)source;
		if ( source->next_effect != NULL )
		{
			last_effect = (AUDIO_Effect_TypeDef *)Sound_Apply_Effect(source->next_effect);
			dest->OutFunc(dest->out_buf,last_effect->out_buf,start_sample,source->block_size,source->channel_out);
		}
	}
	else
		dest->OutFunc(dest->out_buf,source->in_buf,start_sample,source->block_size,source->channel_out);
}

ITCM_AREA_CODE void Do_Audio(uint32_t start_sample)
{
AUDIO_Source_TypeDef *source;
AUDIO_Dest_TypeDef *dest;

#ifdef LCD_SS_GPIO_Port
	HAL_GPIO_WritePin(LCD_SS_GPIO_Port, LCD_SS_Pin, GPIO_PIN_SET);
#endif // #ifdef LCD_SS_GPIO_Port
	audio_pipe_time_start = DWT->CYCCNT;

	source = AudioSourceLeft;
	if ( source == NULL )
		return;
	dest = AudioDestLeft;
	if (( dest == NULL) || ( dest->out_buf == NULL) || ( dest->in_buf == NULL) || ( dest->OutFunc == NULL ))
		return;
	while(source != NULL )
	{
		if ( (source->flags == SOURCE_ENABLED ) )
			audio_gen(source,dest,start_sample,SOUND_SOURCE_IS_SYNTH);
		source = (AUDIO_Source_TypeDef *)source->next_source;
	}
	audio_pipe_time = (DWT->CYCCNT - audio_pipe_time_start) / (HSI_CLOCK / 1000000);
#ifdef LCD_SS_GPIO_Port
	HAL_GPIO_WritePin(LCD_SS_GPIO_Port, LCD_SS_Pin, GPIO_PIN_RESET);
#endif // #ifdef LCD_SS_GPIO_Port
}

#endif // #ifdef SOUND_ENGINE_ENABLED

