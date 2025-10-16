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
#include "../sound.h"
#ifdef SOUND_ENABLED

#include "ringmod.h"

ITCM_AREA_CODE static float ringmod_internal_effect(uint32_t *ring,float input )
{
RINGMOD_Effect_TypeDef *ringmod	= (RINGMOD_Effect_TypeDef *)ring;
	// Update carrier phase
	ringmod->carrierPhase += ringmod->phaseIncrement;
	if (ringmod->carrierPhase >= 2.0f * PI)
		ringmod->carrierPhase -= 2.0f * PI;
	return input * sinf(ringmod->carrierPhase);      // Ring modulation
}

ITCM_AREA_CODE static float ringmod_external_effect(uint32_t *ring,float input )
{
RINGMOD_Effect_TypeDef *ringmod	= (RINGMOD_Effect_TypeDef *)ring;
	// Update carrier phase
	ringmod->carrierPhase += ringmod->phaseIncrement;
	if (ringmod->carrierPhase >= 2.0f * PI)
		ringmod->carrierPhase -= 2.0f * PI;
	return input * sinf(ringmod->carrierPhase);      // Ring modulation
}

ITCM_AREA_CODE void Effect_RingMod_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
RINGMOD_Effect_TypeDef *ringmod = (RINGMOD_Effect_TypeDef *)effect->private_data;
	ringmod->carrierFrequency = RINGMOD_DEFAULT_CARRIER_FREQ;
	if ( ringmod->sample_rate == 0 )
		ringmod->sample_rate = DEFAULT_SAMPLE_FREQUENCY;
	ringmod->phaseIncrement = 2.0f * PI * ringmod->carrierFrequency / ringmod->sample_rate;

	if (( ringmod->status & RINGMODE_STATUS_INTERNAL) == RINGMODE_STATUS_INTERNAL)
		ringmod->ringmod_effect = ringmod_internal_effect;
	else
		ringmod->ringmod_effect = ringmod_external_effect;
	ringmod->status |= SOUND_EFFECT_INITIALIZED;
    effect->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_RingMod(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
RINGMOD_Effect_TypeDef *ringmod = (RINGMOD_Effect_TypeDef *)effect->private_data;

	if ((( ringmod->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( ringmod == NULL ))
		return;
	for ( i=0;i<SOUND_BLOCK_SIZE;i++)
	{
		if (( ringmod->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			effect->out_buf[i + start_sample] = (q15_t ) ringmod->ringmod_effect((uint32_t *)ringmod,(float )effect->in_buf[i]) + effect->out_device;
		else
			effect->out_buf[i + start_sample]  = effect->in_buf[i]+effect->out_device;
	}
}

#endif // #ifdef SOUND_ENABLED
