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
#include "../sound.h"

#ifdef SOUND_ENABLED
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

float moog_f_effect(MOOG_F_Effect_TypeDef* moog_f, float input)
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
    return moog_f->y4;
}

ITCM_AREA_CODE void Effect_MOOG_F_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
MOOG_F_Effect_TypeDef *moog_f = (MOOG_F_Effect_TypeDef *)effect->private_data;
	moog_f->y1 = moog_f->y2 = moog_f->y3 = moog_f->y4 = 0.0f;
	moog_f->g = 0.1f;
	moog_f->k = 0.0f;

	if ( moog_f->cutoffFrequency == NULL )
		return;
	if ( moog_f->resonance == NULL )
		return;
	if ( moog_f->lfo_rate == NULL )
		return;
	if ( moog_f->sample_rate == 0 )
		moog_f->sample_rate = DEFAULT_SAMPLE_FREQUENCY;

	update_g_and_k(moog_f);
	moog_f->status |= SOUND_EFFECT_INITIALIZED;
    effect->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_MOOG_F(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
MOOG_F_Effect_TypeDef *moog_f = (MOOG_F_Effect_TypeDef *)effect->private_data;

	if ((( moog_f->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( moog_f == NULL ))
		return;
	for ( i=0;i<SOUND_BLOCK_SIZE;i++)
	{
		if (( moog_f->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			effect->out_buf[i + start_sample] = (q15_t ) moog_f_effect(moog_f,(float )effect->in_buf[i]) + effect->out_device;
		else
			effect->out_buf[i + start_sample]  = effect->in_buf[i]+effect->out_device;
	}
}
/*


void moog_set_params(MOOG_F_Effect_TypeDef *f, float cutoff, float resonance, float fs)
{
    // Limit cutoff to Nyquist
    if (cutoff > fs * 0.49f) cutoff = fs * 0.49f;
    if (cutoff < 20.0f) cutoff = 20.0f;

    // g = tan(π * fc / fs)
    f->g = tanf(M_PI * cutoff / fs);
    if (f->g > 10.0f) f->g = 10.0f; // stability

    // Resonance: 0.0 (none) to ~3.9 (self-oscillation)
    resonance = (resonance < 0.0f) ? 0.0f : (resonance > 4.0f ? 4.0f : resonance);
    f->k = 4.0f * resonance;
}

 void Audio_Init(void) {
    moog_init(&filter_l);
    moog_init(&filter_r);
    // Set initial params: 1 kHz cutoff, resonance = 0.8
    moog_set_params(&filter_l, 1000.0f, 0.8f, 48000.0f);
    moog_set_params(&filter_r, 1000.0f, 0.8f, 48000.0f);
}

// Called on DMA half-complete & complete
void Process_Audio_Buffer(void) {
    for (int i = 0; i < AUDIO_BUFFER_SIZE; i += 2) {
        // Convert Q15 to float [-1.0, 1.0]
        float in_l = (float)audio_rx_buffer[i] / 32768.0f;
        float in_r = (float)audio_rx_buffer[i + 1] / 32768.0f;

        // Process
        float out_l = moog_process(&filter_l, in_l);
        float out_r = moog_process(&filter_r, in_r);

        // Clamp to [-1, 1] to prevent overflow
        if (out_l > 1.0f) out_l = 1.0f;
        if (out_l < -1.0f) out_l = -1.0f;
        if (out_r > 1.0f) out_r = 1.0f;
        if (out_r < -1.0f) out_r = -1.0f;

        // Convert back to Q15
        audio_tx_buffer[i]     = (int16_t)(out_l * 32767.0f);
        audio_tx_buffer[i + 1] = (int16_t)(out_r * 32767.0f);
    }
}

Using LFO
        // Get LFO value: [-1, 1]
        float lfo_val = lfo_process(&lfo);

        // Map LFO to cutoff range
        float mod_amount = lfo_val * lfo.depth; // [-depth, +depth]
        float cutoff_range = cutoff_max - cutoff_min;
        float current_cutoff = base_cutoff + mod_amount * (cutoff_range * 0.5f);

 */
#endif // #ifdef SOUND_ENABLED
