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
 * filter_fir.c
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "filter_fir.h"
#include <math.h>

// Math constants (if not defined)
#ifndef M_PI_F
#define M_PI_F 3.14159265358979323846f
#endif

static float apply_window(uint32_t n, uint32_t taps, fir_window_type_t window)
{
    float alpha, beta;

    switch (window) {
        case FIR_WINDOW_HAMMING:
            return 0.54f - 0.46f * cosf(2.0f * M_PI_F * n / (taps - 1));

        case FIR_WINDOW_HANN:
            return 0.5f * (1.0f - cosf(2.0f * M_PI_F * n / (taps - 1)));

        case FIR_WINDOW_BLACKMAN:
            alpha = 0.16f;
            beta = 0.5f * (1.0f - alpha);
            return alpha + beta * cosf(2.0f * M_PI_F * n / (taps - 1))
                   + 0.5f * alpha * cosf(4.0f * M_PI_F * n / (taps - 1));

        default:
            return 1.0f;
    }
}

static float sinc(float x)
{
    if (fabsf(x) < 1e-6f) {
        return 1.0f;
    }
    return sinf(M_PI_F * x) / (M_PI_F * x);
}

int fir_generate_coeffs(float *coeffs, uint32_t taps,
                       fir_filter_type_t type,
                       float fs, float f1, float f2,
                       fir_window_type_t window)
{
    // Validate parameters
    if (!coeffs || taps < 4 || taps > FIR_TAPS || (taps & 1)) {
        return -1; // Must be even, 4-128 taps
    }

    if (f1 <= 0.0f || f1 >= fs * 0.5f) {
        return -1; // Invalid frequency
    }

    if (type == FIR_TYPE_BANDPASS) {
        if (f2 <= f1 || f2 >= fs * 0.5f) {
            return -1;
        }
    }

    float *temp_coeffs = coeffs;
    uint32_t center = taps / 2;

    // Generate ideal impulse response
    for (uint32_t n = 0; n < taps; n++) {
        float x = (float)(n - center);

        switch (type) {
            case FIR_TYPE_LOWPASS: {
                float fc_norm = f1 / fs; // Normalized cutoff (0-0.5)
                temp_coeffs[n] = 2.0f * fc_norm * sinc(2.0f * fc_norm * x);
                break;
            }

            case FIR_TYPE_HIGHPASS: {
                float fc_norm = f1 / fs;
                temp_coeffs[n] = (n == center) ? (1.0f - 2.0f * fc_norm) :
                                 -2.0f * fc_norm * sinc(2.0f * fc_norm * x);
                break;
            }

            case FIR_TYPE_BANDPASS: {
                float f_low_norm = f1 / fs;
                float f_high_norm = f2 / fs;
                temp_coeffs[n] = 2.0f * f_high_norm * sinc(2.0f * f_high_norm * x)
                               - 2.0f * f_low_norm * sinc(2.0f * f_low_norm * x);
                break;
            }
        }

        // Apply window
        temp_coeffs[n] *= apply_window(n, taps, window);
    }

    // Normalize gain
    float gain = 0.0f;
    if (type == FIR_TYPE_LOWPASS || type == FIR_TYPE_BANDPASS) {
        // Normalize at DC (0 Hz)
        for (uint32_t i = 0; i < taps; i++) {
            gain += temp_coeffs[i];
        }
    } else { // HIGHPASS
        // Normalize at Nyquist (Fs/2)
        for (uint32_t i = 0; i < taps; i++) {
            gain += temp_coeffs[i] * ((i & 1) ? -1.0f : 1.0f);
        }
    }

    if (fabsf(gain) > 1e-6f) {
        for (uint32_t i = 0; i < taps; i++) {
            temp_coeffs[i] /= gain;
        }
    }

    return 0;
}

ITCM_AREA_CODE void Effect_FIR_Init(uint32_t *effect_s)
{
FIR_Effect_TypeDef *fir = (FIR_Effect_TypeDef *)effect_s;

	if ( fir->block_size == 0 )
		fir->block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	if ( fir->sample_rate == 0 )
		fir->sample_rate = Sound_Sample_Frequency;
	for (int i = 0; i < fir->num_taps; i++)
		fir->fir_coeffs.coeffs[i] = fir->coeffs[fir->num_taps - 1 - i];  // Reverse order
    memset(fir->fir_coeffs.delay_line, 0, sizeof(fir->fir_coeffs.delay_line));
    fir->fir_coeffs.write_index = 0;
    fir->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE static q15_t fir_effect(FIR_Effect_TypeDef *fir, float input)
{
    const float *coeffs = fir->fir_coeffs.coeffs;
    float *delay_line = fir->fir_coeffs.delay_line;
    uint16_t write_idx = fir->fir_coeffs.write_index;
    const uint16_t taps = fir->num_taps;
    float sample = input;

    delay_line[write_idx] = sample;

    // Compute dot product: y[n] = sum_{k=0}^{N-1} h[k] * x[n-k]
    float acc = 0.0f;
    uint16_t read_idx = write_idx;

    for (uint16_t k = 0; k < taps; k++) {
        acc += coeffs[k] * delay_line[read_idx];
        if (read_idx == 0) {
            read_idx = taps - 1;
        } else {
            read_idx--;
        }
    }

    sample = acc;

    // Update write index (circular buffer)
    if (++write_idx >= taps) {
        write_idx = 0;
    }
	return __FLOAT_2_Q15(sample);
}

ITCM_AREA_CODE void Effect_FIR(uint32_t *effect_s)
{
uint32_t	i;
FIR_Effect_TypeDef *fir = (FIR_Effect_TypeDef *)effect_s;

	if ((( fir->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( fir == NULL ))
		return;
	fir->time_start = DWT->CYCCNT;

	if (( fir->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
	{
		for ( i=0;i<fir->block_size;i++)
			fir->out_buf[i] = (q15_t ) fir_effect(fir,__Q15_2_FLOAT(fir->in_buf[i]));
	}
	else
	{
		for ( i=0;i<fir->block_size;i++)
			fir->out_buf[i]  = fir->in_buf[i];
	}

	if (( fir->flags & FIR_UPDATE_PARAMS) == FIR_UPDATE_PARAMS)
	{
		fir->flags &= ~FIR_UPDATE_PARAMS;
	}
	fir->effect_time = (DWT->CYCCNT - fir->time_start) / (HSI_CLOCK / 1000000);
}
#endif //#ifdef SOUND_ENGINE_ENABLED


