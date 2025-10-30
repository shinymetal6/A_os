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
 * filter_feedback_comb.c
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
#include "filter_feedback_comb.h"
#include <math.h>

ITCM_AREA_CODE void feedback_comb_init(uint32_t *effect_s)
{
FEEDBACK_COMB_Effect_TypeDef *feedback_comb = (FEEDBACK_COMB_Effect_TypeDef *)effect_s;
    for (int i = 0; i < feedback_comb->comb_delay_samples; i++)
    	feedback_comb->delay_line[i] = 0.0f;
    feedback_comb->delay_ptr = 0;
    feedback_comb->comb_feedback_gain = COMB_FEEDBACK_DEFAULT_GAIN;
}

ITCM_AREA_CODE static q15_t feedback_comb_effect(FEEDBACK_COMB_Effect_TypeDef *feedback_comb, float input)
{
        // Read delayed output: y[n-D]
float delayed = feedback_comb->delay_line[feedback_comb->delay_ptr];

// Compute current output: y[n] = x[n] + g * y[n-D]
float out = input + feedback_comb->comb_feedback_gain * delayed;

	// Optional: limit to prevent clipping (soft clip)
	if (out > 1.0f)
		out = 1.0f;
	else if (out < -1.0f)
		out = -1.0f;

	// Store output in delay line for next use
	feedback_comb->delay_line[feedback_comb->delay_ptr] = out;

	// Update circular pointer
	feedback_comb->delay_ptr++;
	if (feedback_comb->delay_ptr >= feedback_comb->comb_delay_samples)
		feedback_comb->delay_ptr = 0;
	return __FLOAT_2_Q15(out);

}

ITCM_AREA_CODE void Effect_FeedbackComb(uint32_t *effect_s)
{
uint32_t	i;
FEEDBACK_COMB_Effect_TypeDef *feedback_comb = (FEEDBACK_COMB_Effect_TypeDef *)effect_s;

	if ((( feedback_comb->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( feedback_comb == NULL ))
		return;
	for ( i=0;i<feedback_comb->block_size;i++)
	{
		if (( feedback_comb->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			feedback_comb->out_buf[i] = (q15_t ) feedback_comb_effect(feedback_comb,__Q15_2_FLOAT(feedback_comb->in_buf[i]));
		else
			feedback_comb->out_buf[i]  = feedback_comb->in_buf[i];
	}
	if (( feedback_comb->flags & FEEDBECK_COMB_UPDATE_PARAMS) == FEEDBECK_COMB_UPDATE_PARAMS)
	{
		feedback_comb_init(effect_s);
		feedback_comb->flags &= ~FEEDBECK_COMB_UPDATE_PARAMS;
	}
}
#endif // #ifdef SOUND_ENABLED



