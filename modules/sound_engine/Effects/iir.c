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
 * iir.c
 *
 *  Created on: Apr 30, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "iir.h"

ITCM_AREA_CODE static void calculateBiquadCoefficients(BiquadFilter* bq, FilterType type, float cutoffFreq, float bw, float sample_rate)
{
    // Normalize the cutoff frequency
    float w0 = 2.0f * M_PI * cutoffFreq / sample_rate;
    float sinW0 = sinf(w0);
    float cosW0 = cosf(w0);

    // Precompute alpha for low-pass, high-pass, and band-pass filters
    float alpha = sinW0 / (2.0f * 1.0f); // Q = 1.0 for Butterworth filter

    // Precompute common terms
    float a0 = 1.0f + alpha;

    // Calculate coefficients based on filter type
    switch (type) {
        case FILTER_TYPE_LOW_PASS:
            bq->b0 = (1.0f - cosW0) / (2.0f * a0);
            bq->b1 = (1.0f - cosW0) / a0;
            bq->b2 = bq->b0;
            bq->a1 = (-2.0f * cosW0) / a0;
            bq->a2 = (1.0f - alpha) / a0;
            break;

        case FILTER_TYPE_HIGH_PASS:
            bq->b0 = (1.0f + cosW0) / (2.0f * a0);
            bq->b1 = -(1.0f + cosW0) / a0;
            bq->b2 = bq->b0;
            bq->a1 = (-2.0f * cosW0) / a0;
            bq->a2 = (1.0f - alpha) / a0;
            break;

        case FILTER_TYPE_BAND_PASS:
            // Normalize bandwidth
            float bwNorm = 2.0f * M_PI * bw / sample_rate;
            alpha = sinW0 * sinh((logf(2.0f) / 2.0f) * bwNorm * w0 / sinW0);

            a0 = 1.0f + alpha;
            bq->b0 = alpha / a0;
            bq->b1 = 0.0f;
            bq->b2 = -alpha / a0;
            bq->a1 = (-2.0f * cosW0) / a0;
            bq->a2 = (1.0f - alpha) / a0;
            break;

        case FILTER_TYPE_NOTCH:
            // Normalize bandwidth
            bwNorm = 2.0f * M_PI * bw / sample_rate;
            alpha = sinW0 * sinh((logf(2.0f) / 2.0f) * bwNorm * w0 / sinW0);

            a0 = 1.0f + alpha;
            bq->b0 = 1.0f / a0;
            bq->b1 = (-2.0f * cosW0) / a0;
            bq->b2 = 1.0f / a0;
            bq->a1 = (-2.0f * cosW0) / a0;
            bq->a2 = (1.0f - alpha) / a0;
            break;

        default:
            // Default to low-pass if an invalid type is provided
            bq->b0 = (1.0f - cosW0) / (2.0f * a0);
            bq->b1 = (1.0f - cosW0) / a0;
            bq->b2 = bq->b0;
            bq->a1 = (-2.0f * cosW0) / a0;
            bq->a2 = (1.0f - alpha) / a0;
            break;
    }
}

ITCM_AREA_CODE static q15_t iir_effect(IIR_Effect_TypeDef *iir, float input)
{
float sample = input;

	// Process through each biquad stage
	for (uint8_t j = 0; j < IIR_NUM_BIQUADS; j++) {
		BiquadFilter *bq = &iir->biquads[j];

		// Apply the IIR difference equation:
		// y[n] = b0 * x[n] + b1 * x[n-1] + b2 * x[n-2] - a1 * y[n-1] - a2 * y[n-2]
		float y0 = bq->b0 * input + bq->b1 * bq->x1 + bq->b2 * bq->x2
				 - bq->a1 * bq->y1 - bq->a2 * bq->y2;

		// Update filter state
		bq->x2 = bq->x1;
		bq->x1 = sample;
		bq->y2 = bq->y1;
		bq->y1 = y0;

		// Pass the output of this stage as the input to the next stage
		sample = y0;
	}

	// Write output sample
	return __FLOAT_2_Q15(sample);
}

ITCM_AREA_CODE void Effect_IIR_Init(uint32_t *effect_s)
{
IIR_Effect_TypeDef *iir = (IIR_Effect_TypeDef *)effect_s;

	if ( iir->synth_block_size == 0 )
		iir->synth_block_size = DEFAULT_HALF_NUMBER_OF_AUDIO_SAMPLES;
	if ( iir->sample_rate == 0 )
		iir->sample_rate = Sound_Sample_Frequency;
	for (uint8_t i = 0; i < IIR_NUM_BIQUADS; i++) {
		calculateBiquadCoefficients(&iir->biquads[i], iir->filterType, iir->cutoffFrequency, iir->bandwidth,iir->sample_rate);
		iir->biquads[i].x1 = iir->biquads[i].x2 = 0.0f;
		iir->biquads[i].y1 = iir->biquads[i].y2 = 0.0f;
	}
	if ( iir->cutoffFrequency == 0.0F )
			iir->cutoffFrequency = 1000.0F;
	if ( iir->bandwidth == 0.0F )
			iir->bandwidth = 200.0F;
	iir->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_IIR_SetParams(uint32_t *effect_s,uint32_t *params )
{
IIR_Effect_Params_TypeDef *new_params = (IIR_Effect_Params_TypeDef *)params;
IIR_Effect_TypeDef *iir = (IIR_Effect_TypeDef *)effect_s;

	iir->cutoffFrequency = new_params->cutoffFrequency;
	iir->bandwidth = new_params->bandwidth;
	iir->filterType = new_params->filterType;
	iir->flags |= IIR_UPDATE_PARAMS;
}


ITCM_AREA_CODE void Effect_IIR(uint32_t *effect_s)
{
uint32_t	i;
IIR_Effect_TypeDef *iir = (IIR_Effect_TypeDef *)effect_s;

	if ((( iir->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( iir == NULL ))
		return;
	for ( i=0;i<iir->synth_block_size;i++)
	{
		if (( iir->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			iir->effect_out_buf[i] = (q15_t ) iir_effect(iir,__Q15_2_FLOAT(iir->effect_in_buf[i]));
		else
			iir->effect_out_buf[i]  = iir->effect_in_buf[i];
	}
	if (( iir->flags & IIR_UPDATE_PARAMS) == IIR_UPDATE_PARAMS)
	{
		Effect_IIR_Init(effect_s);
		iir->flags &= ~IIR_UPDATE_PARAMS;
	}
}
#endif // #ifdef SOUND_ENABLED
