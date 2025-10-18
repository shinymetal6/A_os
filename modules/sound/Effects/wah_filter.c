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
 * wah_filter.c
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"
#ifdef SOUND_ENABLED

#include "wah_filter.h"

/*
ITCM_AREA_CODE void Wah_Set_Params(WAH_F_Effect_TypeDef *wah, float min_fc, float max_fc, float res, float sens)
{
    wah->f_min_cutoff = (min_fc < 50.0f) ? 50.0f : min_fc;
    wah->f_max_cutoff = (max_fc > 8000.0f) ? 8000.0f : max_fc;
    wah->f_resonance = (res < 0.0f) ? 0.0f : (res > 0.99f ? 0.99f : res);
    wah->f_sensitivity = sens;
}
*/
ITCM_AREA_CODE static void wah_set_params(WAH_F_Effect_TypeDef *wah)
{

    if (( *wah->attack == 0 ) | ( *wah->attack > 100 ))
    	wah->f_attack = 1.0f - expf(-1.0f / (0.0001f * wah->sample_rate));   // ~0.1 ms f_attack
    else
       	wah->f_attack = 1.0f - expf(-1.0f / (*wah->attack * 0.000001f * wah->sample_rate));

    if (( *wah->release == 0 ) || ( *wah->release < 100 ))
    	wah->f_release = 1.0f - expf(-1.0f / (0.001f * wah->sample_rate));   // ~1 ms f_release
    else
    	wah->f_release = 1.0f - expf(-1.0f / (*wah->release * 0.00001f * wah->sample_rate));   // ~1 ms f_release

    if (( *wah->min_cutoff < 300 ) || ( *wah->min_cutoff > 2000 ))
    	wah->f_min_cutoff = 300.0f;
    else
    	wah->f_min_cutoff = (float )*wah->min_cutoff;

    if (( *wah->max_cutoff < 300 ) || ( *wah->max_cutoff > 2000 ))
    	wah->f_max_cutoff = 2000.0f;
    else
    	wah->f_max_cutoff = (float )*wah->max_cutoff;

    if (( *wah->resonance == 0 ) || ( *wah->resonance > 1 ))
    	wah->f_resonance = 0.85f;
    else
    	wah->f_resonance = (float )*wah->resonance*0.01F;

    if (( *wah->sensitivity == 0 ) || ( *wah->sensitivity > 1 ))
    	wah->f_sensitivity = 1.0f;
    else
    	wah->f_sensitivity = (float )*wah->sensitivity*0.01F;

    if ( wah->sample_rate == 0 )
    	wah->sample_rate = DEFAULT_SAMPLE_FREQUENCY;

    wah->y1 = wah->y2 = wah->y3 = wah->y4 = 0.0f;
}

ITCM_AREA_CODE static q15_t wah_process(WAH_F_Effect_TypeDef *wah, float input)
{
    const float fs = wah->sample_rate;

    // --- Envelope follower ---
    float abs_in = (input > 0.0f) ? input : -input;
    if (abs_in > wah->env) {
        wah->env += wah->f_attack * (abs_in - wah->env);
    } else {
        wah->env += wah->f_release * (abs_in - wah->env);
    }

    // Apply f_sensitivity
    float env_scaled = wah->env * wah->f_sensitivity;
    if (env_scaled > 1.0f) env_scaled = 1.0f;

    // Map envelope to cutoff: quiet → min, loud → max
    float cutoff = wah->f_min_cutoff + env_scaled * (wah->f_max_cutoff - wah->f_min_cutoff);

    // Update filter g = tan(π * fc / fs)
    float g = tanf(M_PI * cutoff / fs);
    if (g > 10.0f) g = 10.0f;
    float k = 4.0f * wah->f_resonance;
    if (k > 4.0f) k = 4.0f;

    // --- Moog ZDF filter (4-pole) ---
    float denom = 1.0f + g * (1.0f + g * (1.0f + g * (1.0f + g)));
    float u = (input - k * wah->y4) / denom;

    float stage = tanhf(u + g * wah->y1); wah->y1 = stage;
    stage = tanhf(stage + g * wah->y2);   wah->y2 = stage;
    stage = tanhf(stage + g * wah->y3);   wah->y3 = stage;
    stage = tanhf(stage + g * wah->y4);   wah->y4 = stage;

    return __FLOAT_2_Q15(wah->y4);
}

ITCM_AREA_CODE void Effect_Wah_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
WAH_F_Effect_TypeDef *wah = (WAH_F_Effect_TypeDef *)effect->private_data;

	if (( wah->min_cutoff == NULL ) || ( wah->max_cutoff == NULL ) || ( wah->resonance == NULL ) || ( wah->sensitivity == NULL ) || ( wah->attack == NULL ) || ( wah->release == NULL ))
		return;
	if ( wah->sample_rate == 0 )
		wah->sample_rate = DEFAULT_SAMPLE_FREQUENCY;
	wah->env = 0.0f;
	wah_set_params(wah);

    wah->status |= SOUND_EFFECT_INITIALIZED;
    effect->status |= SOUND_EFFECT_INITIALIZED;
}


ITCM_AREA_CODE void Effect_Wah(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
WAH_F_Effect_TypeDef *wah = (WAH_F_Effect_TypeDef *)effect->private_data;

	if ((( wah->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( wah == NULL ))
		return;
	for ( i=0;i<SOUND_BLOCK_SIZE;i++)
	{
		if (( wah->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
		{
			wah_set_params(wah);
			effect->out_buf[i + start_sample] = wah_process(wah,__Q15_2_FLOAT(effect->in_buf[i]));
		}
		else
			effect->out_buf[i + start_sample]  = effect->in_buf[i];
	}
}

#endif // #ifdef SOUND_ENABLED
