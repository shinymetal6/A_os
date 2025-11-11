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
 * vibrato.c
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "vibrato.h"
#include <math.h>

ITCM_AREA_CODE void Effect_VIBRATO_Init(uint32_t *effect_s)
{
VIBRATO_Effect_TypeDef *vibrato = (VIBRATO_Effect_TypeDef *)effect_s;
	if ( vibrato->lfoFreq == NULL )
		return;
	if ( vibrato->depth == NULL )
		return;
	if ( *vibrato->lfoFreq > TREMOLO_MAX_FREQ_LFO )
		vibrato->f_lfoFreq = TREMOLO_MAX_FREQ_LFO;
	else
		vibrato->f_lfoFreq = (float )*vibrato->lfoFreq;
	if ( *vibrato->depth > TREMOLO_MAX_DEPTH )
		vibrato->f_lfoDepth = TREMOLO_MAX_DEPTH;
	else
		vibrato->f_lfoDepth = (float )*vibrato->depth;

	if ( vibrato->block_size == 0 )
		vibrato->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	vibrato->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_VIBRATO(uint32_t *effect_s)
{
uint32_t	i;
VIBRATO_Effect_TypeDef *vibrato = (VIBRATO_Effect_TypeDef *)effect_s;

	if ((( vibrato->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( vibrato == NULL ))
		return;
	vibrato->time_start = DWT->CYCCNT;
	if (( vibrato->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		for ( i=0;i<vibrato->block_size;i++)
		{
			vibrato->vibrato_maxDelay  = (uint16_t )(((float )*vibrato->maxDelay*vibrato->sample_rate)/1000.0F);
			uint16_t delaySamples = (uint16_t)(1+(vibrato->vibrato_maxDelay/2)*(1-(vibrato->f_lfoDepth * cos(2*PI*vibrato->phase))));
			float prevSample = __Q15_2_FLOAT(vibrato->in_buf[(i + vibrato->block_size - delaySamples) % vibrato->block_size]);
			vibrato->out_buf[i] = __FLOAT_2_Q15(prevSample);
			vibrato->phase =  fmodf(vibrato->phase + vibrato->f_lfoFreq / vibrato->sample_rate,1);
		}
	}
	else
	{
		for ( i=0;i<vibrato->block_size;i++)
			vibrato->out_buf[i]  = vibrato->in_buf[i];
	}
	vibrato->effect_time = (DWT->CYCCNT - vibrato->time_start) / (HSI_CLOCK / 1000000);
}

#endif //#ifdef SOUND_ENGINE_ENABLED

