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
 * flanger.c
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
#include "flanger.h"
#include <math.h>

ITCM_AREA_CODE void Effect_FLANGER_Init(uint32_t *effect_s)
{
	FLANGER_Effect_TypeDef *flanger = (FLANGER_Effect_TypeDef *)effect_s;
	if ( flanger->lfoFreq == NULL )
		return;
	if ( flanger->depth == NULL )
		return;
	if ( *flanger->lfoFreq > TREMOLO_MAX_FREQ_LFO )
		flanger->f_lfoFreq = TREMOLO_MAX_FREQ_LFO;
	else
		flanger->f_lfoFreq = (float )*flanger->lfoFreq;
	if ( *flanger->depth > TREMOLO_MAX_DEPTH )
		flanger->f_lfoDepth = TREMOLO_MAX_DEPTH;
	else
		flanger->f_lfoDepth = (float )*flanger->depth;

	if ( flanger->block_size == 0 )
		flanger->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	flanger->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_FLANGER(uint32_t *effect_s)
{
uint32_t	i;
FLANGER_Effect_TypeDef *flanger = (FLANGER_Effect_TypeDef *)effect_s;

	if ((( flanger->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( flanger == NULL ))
		return;
	flanger->time_start = DWT->CYCCNT;
	if (( flanger->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		for ( i=0;i<flanger->block_size;i++)
		{
			float sample = __Q15_2_FLOAT (flanger->out_buf[i]);
			uint16_t delaySamples = (uint16_t)(1+(flanger->flanger_maxDelay/2)*(1-(flanger->f_lfoDepth * cos(2*PI*flanger->phase))));
			float prevSample = __Q15_2_FLOAT (flanger->in_buf[(i + flanger->block_size - delaySamples) % flanger->block_size]);
			flanger->out_buf[i] = __FLOAT_2_Q15((sample+prevSample)/2);
			flanger->phase =  fmodf(flanger->phase + flanger->f_lfoFreq / flanger->sample_rate,1);
		}
	}
	else
	{
		for ( i=0;i<flanger->block_size;i++)
			flanger->out_buf[i]  = flanger->in_buf[i];
	}
	flanger->effect_time = (DWT->CYCCNT - flanger->time_start) / (HSI_CLOCK / 1000000);
}

#endif //#ifdef SOUND_ENGINE_ENABLED


