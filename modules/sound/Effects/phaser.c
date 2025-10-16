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
 * phaser.c
 *
 *  Created on: Apr 29, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"

#ifdef SOUND_ENABLED
#include "effects.h"
#include "phaser.h"

ITCM_AREA_CODE static float phaser_all_pass_filter(PHASER_Effect_TypeDef *phaser,float input, float feedback)
{
    float delayed = phaser->buffer[phaser->write_pos];
    float output = feedback * input + delayed - feedback * phaser->buffer[phaser->write_pos];
    phaser->buffer[phaser->write_pos] = input;
    phaser->write_pos = (phaser->write_pos + 1) % PHASER_BUFFER_SIZE; // Circular buffer
    return output;
}

ITCM_AREA_CODE static void phaser_update_lfo(PHASER_Effect_TypeDef *phaser)
{
	phaser->lfo_phase = 0.0F;
	if ( phaser->f_lfo_rate == 0.0F)
		phaser->f_lfo_rate = 1.0F;
	else
		phaser->f_lfo_rate = (float )*phaser->lfo_rate;
	phaser->lfo_increment = 2.0f * M_PI * phaser->f_lfo_rate / phaser->sample_rate;
	if ( phaser->depth == 0)
		phaser->f_depth = 0.5F;
	else
		phaser->f_depth = (float )*phaser->depth / FULL_SCALE_F_FACTOR;
	phaser->depth_mul = 1.0F - phaser->f_depth;
	phaser->depth_sum = 1.0F - phaser->depth_mul;

	if ( *phaser->mix == 0.0F)
		phaser->f_mix = 0.5F;
	else
		phaser->f_mix = *phaser->mix / FULL_SCALE_F_FACTOR;
}

// Process one sample
ITCM_AREA_CODE static float phaser_effect(PHASER_Effect_TypeDef *phaser,float input)
{
    // Update LFO phase

	phaser_update_lfo(phaser);
	phaser->lfo_phase += phaser->lfo_increment; // Increment phase (adjust for desired LFO rate) , 0.001f default
    if (phaser->lfo_phase >= 2.0f * M_PI)
    	phaser->lfo_phase -= 2.0f * M_PI;

    // Generate LFO signal (sinusoidal modulation)
    float lfo_signal = sinf(phaser->lfo_phase);

    // Modulate feedback coefficient
    //float feedback = 0.7f + 0.3f * lfo_signal; // Base + modulation
    float feedback = phaser->depth_sum + phaser->depth_mul * lfo_signal; // Base + modulation

    // Apply cascade of all-pass filters
    float wet = input;
    for (int i = 0; i < phaser->allpass_number; i++) { // Cascade of 6 all-pass filters
        wet = phaser_all_pass_filter(phaser,wet, feedback);
    }
    // Mix dry and wet signals
    return phaser->f_mix * input + (1.0F - phaser->f_mix) * wet;  // Simple equal mix

}

ITCM_AREA_CODE void Effect_Phaser_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
PHASER_Effect_TypeDef *phaser = (PHASER_Effect_TypeDef *)effect->private_data;

	phaser->lfo_phase = 0.0F;
	phaser_update_lfo(phaser);

	phaser->allpass_number = PHASER_NUM_ALLPASS;
	phaser->status |= SOUND_EFFECT_INITIALIZED;
    effect->status |= SOUND_EFFECT_INITIALIZED;
}


ITCM_AREA_CODE void Effect_Phaser(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
PHASER_Effect_TypeDef *phaser = (PHASER_Effect_TypeDef *)effect->private_data;

	if ((( phaser->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( phaser == NULL ))
		return;
	for ( i=0;i<SOUND_BLOCK_SIZE;i++)
	{
		if (( phaser->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			effect->out_buf[i + start_sample] = (q15_t ) phaser_effect(phaser,(float )effect->in_buf[i]) + effect->out_device;
		else
			effect->out_buf[i + start_sample]  = effect->in_buf[i]+effect->out_device;
	}
}
#endif // #ifdef SOUND_ENABLED
