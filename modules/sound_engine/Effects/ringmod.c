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
 * ringmod.c
 *
 *  Created on: May 17, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "ringmod.h"

ITCM_AREA_CODE static q15_t ringmod_internal_effect(uint32_t *ring,float input )
{
RINGMOD_Effect_TypeDef *ringmod	= (RINGMOD_Effect_TypeDef *)ring;
	// Update carrier phase
	ringmod->carrierPhase += ringmod->phaseIncrement;
	if (ringmod->carrierPhase >= 2.0f * PI)
		ringmod->carrierPhase -= 2.0f * PI;
	return __FLOAT_2_Q15(input * sinf(ringmod->carrierPhase));      // Ring modulation
}

ITCM_AREA_CODE static q15_t ringmod_external_effect(uint32_t *ring,float input )
{
RINGMOD_Effect_TypeDef *ringmod	= (RINGMOD_Effect_TypeDef *)ring;
	// Update carrier phase
	ringmod->carrierPhase += ringmod->phaseIncrement;
	if (ringmod->carrierPhase >= 2.0f * PI)
		ringmod->carrierPhase -= 2.0f * PI;
	return __FLOAT_2_Q15(input * sinf(ringmod->carrierPhase));      // Ring modulation
}

ITCM_AREA_CODE void Effect_RingMod_Init(uint32_t *effect_s)
{
RINGMOD_Effect_TypeDef *ringmod = (RINGMOD_Effect_TypeDef *)effect_s;

	if ( ringmod->carrierFrequency == NULL )
		return;
	if ( ringmod->synth_block_size == 0 )
		ringmod->synth_block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	ringmod->f_carrierFrequency = (float )*ringmod->carrierFrequency;
	if ( ringmod->f_carrierFrequency > RINGMOD_MAX_CARRIER_FREQ)
		ringmod->f_carrierFrequency = RINGMOD_DEFAULT_CARRIER_FREQ;
	if ( ringmod->sample_rate == 0 )
		ringmod->sample_rate = Sound_Sample_Frequency;
	ringmod->phaseIncrement = 2.0f * PI * ringmod->f_carrierFrequency / ringmod->sample_rate;

	if (( ringmod->status & RINGMODE_STATUS_INTERNAL) == RINGMODE_STATUS_INTERNAL)
		ringmod->ringmod_effect = ringmod_internal_effect;
	else
		ringmod->ringmod_effect = ringmod_external_effect;
	ringmod->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_RingMod(uint32_t *effect_s)
{
uint32_t	i;
RINGMOD_Effect_TypeDef *ringmod = (RINGMOD_Effect_TypeDef *)effect_s;

	if ((( ringmod->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( ringmod == NULL ))
		return;
	for ( i=0;i<ringmod->synth_block_size;i++)
	{
		if (( ringmod->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			ringmod->effect_out_buf[i] = ringmod->ringmod_effect((uint32_t *)ringmod,__Q15_2_FLOAT(ringmod->effect_in_buf[i]));
		else
			ringmod->effect_out_buf[i]  = ringmod->effect_in_buf[i];
	}
}

#endif // #ifdef SOUND_ENGINE_ENABLED
