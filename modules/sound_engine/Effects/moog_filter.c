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
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "moog_filter.h"

void update_g_and_k(MOOG_F_Effect_TypeDef* moog_f)
{
	moog_f->f_cutoffFrequency = (float )*moog_f->cutoffFrequency;
    if (moog_f->f_cutoffFrequency > moog_f->sample_rate * 0.49f)
    	moog_f->f_cutoffFrequency = moog_f->sample_rate * 0.49f;
    if (moog_f->f_cutoffFrequency < 20.0f)
    	moog_f->f_cutoffFrequency = 20.0f;

    // g = tan(π * fc / fs)
    moog_f->g = tanf(M_PI * moog_f->f_cutoffFrequency / moog_f->sample_rate);

    // Stability
	if (moog_f->g > 10.0f)
		moog_f->g = 10.0f;

	moog_f->f_resonance = (float )*moog_f->resonance / FULL_SCALE_F_FACTOR;
	moog_f->f_resonance = (moog_f->f_resonance < 0.0f) ? 0.0f : (moog_f->f_resonance > 4.0f ? 4.0f : moog_f->f_resonance);
	moog_f->k = 4.0f * moog_f->f_resonance;
}

ITCM_AREA_CODE  static q15_t moog_f_effect(MOOG_F_Effect_TypeDef* moog_f, float input)
{
	update_g_and_k(moog_f);
    // Solve feedback: u = (x - k*y4) / (1 + g*(1 + g*(1 + g*(1 + g))))
    const float denom = 1.0f + moog_f->g * (1.0f + moog_f->g * (1.0f + moog_f->g * (1.0f + moog_f->g)));
    float u = (input - moog_f->k * moog_f->y4) / denom;
    // Four nonlinear stages
    float stage = tanhf(u + moog_f->g * moog_f->y1); moog_f->y1 = stage;
    stage = tanhf(stage + moog_f->g * moog_f->y2);   moog_f->y2 = stage;
    stage = tanhf(stage + moog_f->g * moog_f->y3);   moog_f->y3 = stage;
    stage = tanhf(stage + moog_f->g * moog_f->y4);   moog_f->y4 = stage;
    return __FLOAT_2_Q15(moog_f->y4);
}

ITCM_AREA_CODE void Effect_MOOG_F_Init(uint32_t *effect_s)
{
MOOG_F_Effect_TypeDef *moog_f = (MOOG_F_Effect_TypeDef *)effect_s;

	if ( moog_f->cutoffFrequency == NULL )
		return;
	if ( moog_f->resonance == NULL )
		return;
	if ( moog_f->lfo_rate == NULL )
		return;

	moog_f->y1 = moog_f->y2 = moog_f->y3 = moog_f->y4 = 0.0f;
	moog_f->g = 0.1f;
	moog_f->k = 0.0f;

	if ( moog_f->sample_rate == 0 )
		moog_f->sample_rate = Sound_Sample_Frequency;
	if ( moog_f->synth_block_size == 0 )
		moog_f->synth_block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	update_g_and_k(moog_f);
	moog_f->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_MOOG_F(uint32_t *effect_s)
{
uint32_t	i;
MOOG_F_Effect_TypeDef *moog_f = (MOOG_F_Effect_TypeDef *)effect_s;


	if ((( moog_f->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( moog_f == NULL ))
		return;
	for ( i=0;i<moog_f->synth_block_size;i++)
	{
		if (( moog_f->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			moog_f->effect_out_buf[i] = moog_f_effect(moog_f,__Q15_2_FLOAT(moog_f->effect_in_buf[i]));
		else
			moog_f->effect_out_buf[i]  = moog_f->effect_in_buf[i];
	}
}

#endif // #ifdef SOUND_ENABLED
