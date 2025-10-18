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
 * echo.c
 *
 *  Created on: Apr 25, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"
#ifdef SOUND_ENABLED

#include "effects.h"
#include "echo.h"
#ifdef	ECHO_INCLUDED

// Process a block of audio samples through the echo effect
ITCM_AREA_CODE static void echo_process(Echo_Effect_TypeDef *echo, q15_t *input, q15_t *output, uint32_t block_size)
{
    for (int i = 0; i < block_size; i++) {
        // Process through delay line
        q15_t delayed_sample = Sound_Delay_Line(&echo->delay_line, input[i]);

        // Attenuate the delayed signal
        q15_t attenuated_sample = (q15_t)((float)delayed_sample * echo->attenuation);

        // Mix dry and wet signals
        echo->dry_mix = (float )*echo->mix / HALF_SCALE_F_FACTOR;
        echo->wet_mix = 1.0F - echo->dry_mix;
        q15_t mixed_output = (q15_t )(float )(input[i] * echo->dry_mix) + (float )(attenuated_sample * echo->wet_mix);

        // Store the output sample
        output[i] = mixed_output;
    }
}

// Initialize the echo effect
ITCM_AREA_CODE void Effect_Echo_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
Echo_Effect_TypeDef *echo = (Echo_Effect_TypeDef *)effect->private_data;

	if ( echo->sample_rate == 0 )
		echo->sample_rate = DEFAULT_SAMPLE_FREQUENCY;
    echo->dry_mix = (float )*echo->mix / HALF_SCALE_F_FACTOR;
    echo->wet_mix = 1.0F - echo->dry_mix;
    uint32_t delay_samples = (uint32_t)(echo->delay_ms * echo->sample_rate / 1000);
    if (delay_samples > ECHO_MAX_DELAY_LENGTH)
    	delay_samples = ECHO_MAX_DELAY_LENGTH; // Clamp delay length
    Sound_Delay_Line_Init(&echo->delay_line, delay_samples);
    echo->status |= SOUND_EFFECT_INITIALIZED;
    effect->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_Echo(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
Echo_Effect_TypeDef *echo = (Echo_Effect_TypeDef *)effect->private_data;

	if ((( echo->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( echo == NULL ))
		return;
	if (( echo->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
		echo_process(echo,effect->in_buf,effect->out_buf + start_sample,ECHO_BLOCK_SIZE);
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			effect->out_buf[i + start_sample]  = effect->in_buf[i];
	}
}
#endif // #ifdef	ECHO_INCLUDED

#endif // #ifdef SOUND_ENABLED
