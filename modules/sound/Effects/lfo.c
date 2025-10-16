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
 * lfo.c
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
#include "lfo.h"

void lfo_update_phase_inc(LFO_Effect_TypeDef *lfo)
{
    lfo->phase_inc = lfo->f_rate / lfo->sample_rate;
}

void Effect_LFO_init(LFO_Effect_TypeDef *lfo, float sample_rate)
{
    lfo->phase = 0.0f;
	if ( lfo->rate == NULL )
		return;
	if ( lfo->depth == NULL )
		return;

	if ( lfo->sample_rate == 0 )
		lfo->sample_rate = DEFAULT_SAMPLE_FREQUENCY;

	if ( *lfo->rate == 0 )
		lfo->f_rate = 1.0f;
	if ( *lfo->rate > 20 )
		lfo->f_rate = 20.0F;

	if ( *lfo->depth == 0 )
		lfo->f_depth = 0.5f;
	else
		lfo->f_depth = (float )lfo->f_rate / FULL_SCALE_F_FACTOR;

    lfo->waveform = LFO_SINE;
    lfo_update_phase_inc(lfo);
}

void lfo_set_rate(LFO_Effect_TypeDef *lfo, float rate_hz)
{
    if (rate_hz < 0.01f)
    	rate_hz = 0.01f;
    if (rate_hz > 20.0f)
    	rate_hz = 20.0f;
    lfo->f_rate = rate_hz;
    lfo_update_phase_inc(lfo);
}

void lfo_set_depth(LFO_Effect_TypeDef *lfo, float depth)
{
    if (depth < 0.0f) depth = 0.0f;
    if (depth > 1.0f) depth = 1.0f;
    lfo->f_depth = depth;
}

void lfo_set_waveform(LFO_Effect_TypeDef *lfo, lfo_waveform_t wf)
{
    lfo->waveform = wf;
}


// Returns normalized LFO output: [-1.0, 1.0]
float Effect_LFO_process(LFO_Effect_TypeDef *lfo)
{
float out = 0.0f;

    switch (lfo->waveform)
    {
        case LFO_SINE:
            out = sinf(2.0f * M_PI * lfo->phase);
            break;

        case LFO_TRIANGLE: {
            float p = lfo->phase;
            if (p < 0.25f) out = 4.0f * p;
            else if (p < 0.75f) out = 2.0f - 4.0f * p;
            else out = 4.0f * p - 4.0f;
            break;
        }

        case LFO_SAW:
            out = 2.0f * lfo->phase - 1.0f; // rising saw
            break;

        case LFO_SQUARE:
            out = (lfo->phase < 0.5f) ? 1.0f : -1.0f;
            break;
    }

    // Advance phase
    lfo->phase += lfo->phase_inc;
    if (lfo->phase >= 1.0f)
    	lfo->phase -= 1.0f;

    return out;
}

#endif // #ifdef SOUND_ENABLED
