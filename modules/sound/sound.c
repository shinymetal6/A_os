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
 * sound.c
 *
 *  Created on: Apr 19, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef SOUND_ENABLED
#include "sound.h"

#define	SOUND_DELAY_AREA	__attribute__((section(".d2ram"))) __attribute__ ((aligned (32)))
SOUND_DELAY_AREA			q15_t sound_delay_buffer[ECHO_MAX_DELAY_LENGTH];

// Initialize the delay line
ITCM_AREA_CODE void Sound_Delay_Line_Init(Sound_DelayLine_TypeDef *delay_line, uint32_t delay_length)
{
	delay_line->buffer = sound_delay_buffer;
    memset(delay_line->buffer, 0, SOUND_MAX_DELAY_LENGTH);
    delay_line->write_index = 0;
    delay_line->delay_length = delay_length;
}

// Process a sample through the delay line
ITCM_AREA_CODE q15_t Sound_Delay_Line(Sound_DelayLine_TypeDef *delay_line, q15_t input)
{
    // Read from the delay line
    uint32_t read_index = (delay_line->write_index + SOUND_MAX_DELAY_LENGTH - delay_line->delay_length) % SOUND_MAX_DELAY_LENGTH;
    q15_t output = delay_line->buffer[read_index];

    // Write to the delay line
    delay_line->buffer[delay_line->write_index] = input;
    delay_line->write_index = (delay_line->write_index + 1) % SOUND_MAX_DELAY_LENGTH;

    return output;
}

uint32_t 			null_start_sample = 0;

ITCM_AREA_CODE uint8_t Sound_Insert_Effect(MidiSynth_TypeDef *Synth,Effect_TypeDef *effect)
{
	if (( Synth == NULL ) || ( effect == NULL ))
		return 1;

	if ( Synth->effect_s == NULL )
	{
		Synth->effect_s = (uint32_t *)effect;
		effect->pre_effect_s = (uint32_t *)Synth;
		effect->next_effect_s = NULL;

		int16_t	*out_buf = Synth->out_buf;
		Synth->out_buf = effect->in_buf;
		effect->out_buf = out_buf;

		effect->out_device = Synth->out_device;
		Synth->out_device = SYNTH_I2S_OUT;
		if ( effect->effect_init != NULL )
			effect->effect_init((uint32_t *)effect);
		effect->status |= SOUND_EFFECT_INITIALIZED;
	}
	else
	{
		Effect_TypeDef *pre_effect = (Effect_TypeDef *)Synth->effect_s , *current_effect = effect;

		while( pre_effect->next_effect_s != NULL )
			pre_effect = (Effect_TypeDef *)pre_effect->next_effect_s;
		pre_effect->next_effect_s = (uint32_t *)effect;

		current_effect->pre_effect_s = (uint32_t *)pre_effect;
		current_effect->next_effect_s = NULL;

		int16_t	*out_buf = pre_effect->out_buf;
		pre_effect->out_buf = current_effect->in_buf;
		current_effect->out_buf = out_buf;

		current_effect->out_device = pre_effect->out_device;
		pre_effect->out_device = SYNTH_I2S_OUT;
		if ( effect->effect_init != NULL )
			effect->effect_init((uint32_t *)effect);
		effect->status |= SOUND_EFFECT_INITIALIZED;
	}
	return 0;
}

ITCM_AREA_CODE Effect_TypeDef *Sound_Apply_Effect(Effect_TypeDef *effect,uint32_t start_sample)
{
uint8_t		done=0;
uint32_t	st;

	while(done == 0 )
	{
		st = (effect->next_effect_s==NULL) ? start_sample : 0;
		if ( (effect->status & SOUND_EFFECT_INITIALIZED) == SOUND_EFFECT_INITIALIZED)
			effect->effect( (uint32_t *)effect , st);

		if ( effect->next_effect_s != NULL )
			effect = (Effect_TypeDef *)effect->next_effect_s;
		else
			return effect;
	}
	return NULL;
}
#endif
