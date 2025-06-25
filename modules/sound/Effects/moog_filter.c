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
 * moog_filter.c
 *
 *  Created on: Apr 30, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"
#ifdef SOUND_ENABLED

#include "moog_filter.h"

ITCM_AREA_CODE static float getCutoffCoefficient(MOOG_F_Effect_TypeDef* moog_f)
{
    // Map frequency to an index in the lookup table
    uint32_t index = (uint32_t)((moog_f->cutoffFrequency * MOOG_F_NUM_CUTOFF_STEPS) / (MOOG_F_SAMPLE_RATE / 2.0f));
    if (index >= MOOG_F_NUM_CUTOFF_STEPS)
    	index = MOOG_F_NUM_CUTOFF_STEPS - 1; // Clamp index
    return moog_f->cutoffLookupTable[index];
}

ITCM_AREA_CODE static float processMoogSample(MOOG_F_Effect_TypeDef* moog_f,float input, float cutoffK)
{
    // Nonlinear feedback calculation
float saturation = moog_f->feedback - (moog_f->feedback * moog_f->feedback * moog_f->feedback) / 6.0f; // Soft clipping
float inputStage1 = input - moog_f->resonance * saturation;

    // Process each stage of the filter
    moog_f->stage1 += cutoffK * (inputStage1 - moog_f->stage1);
    moog_f->stage2 += cutoffK * (moog_f->stage1 - moog_f->stage2);
    moog_f->stage3 += cutoffK * (moog_f->stage2 - moog_f->stage3);
    moog_f->stage4 += cutoffK * (moog_f->stage3 - moog_f->stage4);

    // Update feedback signal
    moog_f->feedback = moog_f->stage4;

    // Return the output of the last stage
    return moog_f->stage4;
}

ITCM_AREA_CODE static float moog_f_effect(MOOG_F_Effect_TypeDef* moog_f,float input,uint32_t phase)
{
    // Get the current cutoff coefficient from the lookup table
float cutoffK = getCutoffCoefficient(moog_f);

    if ( moog_f->lfo_enabled)
    {
        // Optionally modulate the cutoff frequency and resonance here
        // Example: Modulate cutoff with an LFO
        // float lfoValue = 0.5f * (1.0f + sinf(2.0f * M_PI * i / SAMPLE_RATE));
        // cutoffFrequency = 1000.0f + 500.0f * lfoValue;
        moog_f->lfo_phase = 0.5f * (1.0f + sinf(2.0f * M_PI * phase / MOOG_F_SAMPLE_RATE));
        moog_f->cutoffFrequency = 1000.0f + 500.0f * moog_f->lfo_phase;
        // Recompute cutoff coefficient if cutoffFrequency changes
        cutoffK = getCutoffCoefficient(moog_f);
    }

	// Process each sample through the Moog filter
	return processMoogSample(moog_f,input, cutoffK);
}

ITCM_AREA_CODE void Effect_MOOG_F_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
MOOG_F_Effect_TypeDef *moog_f = (MOOG_F_Effect_TypeDef *)effect->private_data;
	// Reset filter state
	moog_f->stage1 = moog_f->stage2 = moog_f->stage3 = moog_f->stage4 = 0.0f;
	moog_f->feedback = 0.0f;

	// Initialize the cutoff lookup table
	for (uint32_t i = 0; i < MOOG_F_NUM_CUTOFF_STEPS; i++)
	{
		float freq = (float)i * (MOOG_F_SAMPLE_RATE / 2.0f) / MOOG_F_NUM_CUTOFF_STEPS; // Map index to frequency
		moog_f->cutoffLookupTable[i] = tanf(M_PI * freq / MOOG_F_SAMPLE_RATE);         // Precompute k
	}
	moog_f->lfo_phase = 0.0F;
	if ( moog_f->lfo_rate == 0.0F)
		moog_f->lfo_rate = 1.0F;
	moog_f->lfo_increment = 2.0f * M_PI * moog_f->lfo_rate / MOOG_F_SAMPLE_RATE;
}

ITCM_AREA_CODE void Effect_MOOG_F(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
MOOG_F_Effect_TypeDef *moog_f = (MOOG_F_Effect_TypeDef *)effect->private_data;

	for ( i=0;i<SOUND_BLOCK_SIZE;i++)
	{
		if (( effect->status & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			effect->out_buf[i + start_sample] = (q15_t ) moog_f_effect(moog_f,(float )effect->in_buf[i],i) + effect->out_device;
		else
			effect->out_buf[i + start_sample]  = effect->in_buf[i]+effect->out_device;
	}
}
#endif // #ifdef SOUND_ENABLED
