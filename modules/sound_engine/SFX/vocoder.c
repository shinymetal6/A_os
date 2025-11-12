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
 * vocoder.c
 *
 *  Created on: Nov 12, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED

#include "../sound_engine.h"
#include "vocoder.h"
#include <string.h>
#include "vocoder.h"



// Filter instances

// Center frequencies (Hz)
const float center_freqs[NUM_BANDS] = {
    80, 110, 160, 230, 330, 470, 680, 980, 1400, 2000
};

// Q values per band
const float q_values[NUM_BANDS] = {
    2.0f, 2.2f, 2.4f, 2.6f, 2.8f, 3.0f, 3.2f, 3.4f, 3.6f, 3.8f
};
/*
// Envelope followers
float envelopes[NUM_BANDS] = {0};

// Attack/Release coefficients (per sample)
float attack_coef, release_coef;
*/

// Function to design a biquad band-pass filter
void biquad_init_bpf(VOCODER_BiquadFilter_TypeDef* f, float fc, float q, float sample_rate)
{
    float w0 = 2.0f * PI * fc / sample_rate;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float alpha = sin_w0 / (2.0f * q);

    f->b0 = alpha;
    f->b1 = 0.0f;
    f->b2 = -alpha;
    f->a1 = -2.0f * cos_w0;
    f->a2 = 1.0f - alpha;

    // Normalize by a0 (which is 1+alpha)
    float a0_inv = 1.0f / (1.0f + alpha);
    f->b0 *= a0_inv;
    f->b1 *= a0_inv;
    f->b2 *= a0_inv;
    f->a1 *= a0_inv;
    f->a2 *= a0_inv;

    // Zero history
    f->x1 = f->x2 = 0.0f;
    f->y1 = f->y2 = 0.0f;
}

// Apply biquad filter to one sample
float biquad_process(VOCODER_BiquadFilter_TypeDef* f, float x)
{
float y = 	f->b0 * x +
			f->b1 * f->x1 +
			f->b2 * f->x2 -
			f->a1 * f->y1 -
			f->a2 * f->y2;

    // Shift history
    f->x2 = f->x1;
    f->x1 = x;
    f->y2 = f->y1;
    f->y1 = y;
    return y;
}

// Simple envelope follower
float update_envelope(float sample, float env, float attack, float release)
{
float abs_sample = fabsf(sample);
    if (abs_sample > env)
        return env + (abs_sample - env) * attack;
    else
        return env + (abs_sample - env) * release;
}

ITCM_AREA_CODE static void  vocoder_effect(VOCODER_Effect_TypeDef *vocoder)
{
float temp_mod[BLOCK_SIZE];
float temp_car[BLOCK_SIZE];
uint32_t	i,band;
    // Clear output
    for (i = 0; i < vocoder->block_size; i++)
    	vocoder->out_buf[i] = 0.0f;

    // Process each band
    for (band = 0; band < NUM_BANDS; band++)
    {
    	VOCODER_BiquadFilter_TypeDef* f = &vocoder->bpf_filters[band];

        // Filter modulator and extract envelope
        for (i = 0; i < vocoder->block_size; i++)
        {
            temp_mod[i] = biquad_process(f, vocoder->modulation_buf[i]);
            vocoder->envelopes[band] = update_envelope(temp_mod[i], vocoder->envelopes[band], vocoder->attack_coef, vocoder->release_coef);
        }

        // Filter carrier and apply envelope
        for (i = 0; i < vocoder->block_size; i++)
        {
            temp_car[i] = biquad_process(f, vocoder->carrier_buf[i]);
            vocoder->out_buf[i] += temp_car[i] * vocoder->envelopes[band];
        }
    }
}

ITCM_AREA_CODE void Effect_VOCODER_Init(uint32_t *effect_s)
{
VOCODER_Effect_TypeDef *vocoder = (VOCODER_Effect_TypeDef *)effect_s;
uint32_t	i;

	if (( vocoder->attack_time_Msec == NULL) || ( vocoder->release_time_Msec == NULL))
		return;
	if (( vocoder->modulation_buf == NULL) || ( vocoder->carrier_buf == NULL) || ( vocoder->out_buf == NULL))
		return;
	vocoder->attack_time_sec  = (float )*vocoder->attack_time_Msec  / 1000.0F; // 0.01f -> 10 ms
	vocoder->release_time_sec = (float )*vocoder->release_time_Msec / 1000.0F; // 0.1ff -> 100 ms
	vocoder->attack_coef  = 1.0f / (vocoder->attack_time_sec * vocoder->sample_rate);
	vocoder->release_coef = 1.0f / (vocoder->release_time_sec * vocoder->sample_rate);

	// Initialize all BPFs
	for (i = 0; i < NUM_BANDS; i++)
		biquad_init_bpf(&vocoder->bpf_filters[i], center_freqs[i], q_values[i],vocoder->sample_rate);

	// Reset envelopes
	for (i = 0; i < NUM_BANDS; i++) {
		vocoder->envelopes[i] = 0.0f;
	}
	vocoder->status |= SOUND_EFFECT_INITIALIZED;
}


ITCM_AREA_CODE void Effect_VOCODER(uint32_t *effect_s)
{
VOCODER_Effect_TypeDef *vocoder = (VOCODER_Effect_TypeDef *)effect_s;
uint32_t	i;
	if ((( vocoder->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( vocoder == NULL ))
		return;
	vocoder->time_start = DWT->CYCCNT;
	if (( vocoder->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
		vocoder_effect(vocoder);
	else
	{
		for ( i=0;i<vocoder->block_size;i++)
			vocoder->out_buf[i]  = vocoder->modulation_buf[i];
	}
	vocoder->effect_time = (DWT->CYCCNT - vocoder->time_start) / (HSI_CLOCK / 1000000);
}

#endif // #ifdef SOUND_ENGINE_ENABLED
