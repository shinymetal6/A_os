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
 * tremolo.c
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
#include "tremolo.h"
#include <math.h>

ITCM_AREA_CODE void Effect_TREMOLO_Init(uint32_t *effect_s)
{
TREMOLO_Effect_TypeDef *tremolo = (TREMOLO_Effect_TypeDef *)effect_s;
	if ( tremolo->lfoFreq == NULL )
		return;
	if ( tremolo->depth == NULL )
		return;
	if ( *tremolo->lfoFreq > TREMOLO_MAX_FREQ_LFO )
		tremolo->f_lfoFreq = TREMOLO_MAX_FREQ_LFO;
	if ( *tremolo->depth > TREMOLO_MAX_DEPTH )
		tremolo->f_lfoDepth = TREMOLO_MAX_DEPTH;

	if ( tremolo->block_size == 0 )
		tremolo->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	tremolo->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_TREMOLO(uint32_t *effect_s)
{
uint32_t	i;
TREMOLO_Effect_TypeDef *tremolo = (TREMOLO_Effect_TypeDef *)effect_s;

	if ((( tremolo->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( tremolo == NULL ))
		return;
	tremolo->time_start = DWT->CYCCNT;
	if (( tremolo->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		for ( i=0;i<tremolo->block_size;i++)
		{
			float sample = __Q15_2_FLOAT(tremolo->out_buf[i]);
			sample *= (1.0-tremolo->f_lfoDepth) + tremolo->f_lfoDepth * cos(2*PI*tremolo->phase);
			tremolo->out_buf[i] = __FLOAT_2_Q15(sample);
			tremolo->phase =  fmodf(tremolo->phase + tremolo->f_lfoFreq / tremolo->sample_rate,1);
		}
	}
	else
	{
		for ( i=0;i<tremolo->block_size;i++)
			tremolo->out_buf[i]  = tremolo->in_buf[i];
	}
	tremolo->effect_time = (DWT->CYCCNT - tremolo->time_start) / (HSI_CLOCK / 1000000);
}

#endif //#ifdef SOUND_ENGINE_ENABLED
