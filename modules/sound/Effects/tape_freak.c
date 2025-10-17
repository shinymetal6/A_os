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
 * tape_freak.c
 *
 *  Created on: May 17, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"
#ifdef SOUND_ENABLED

#include "tape_freak.h"

ITCM_AREA_CODE static void apply_tape_freak_lpf(TAPE_FREAK_Effect_TypeDef *tape_freak,float32_t* input, float32_t* output, uint32_t blockSize)
{
    arm_biquad_cascade_df2T_f32(&tape_freak->LPF_Instance, input, output, blockSize);
}

ITCM_AREA_CODE static void apply_tape_freak_bass(TAPE_FREAK_Effect_TypeDef *tape_freak,float32_t* input, float32_t* output, uint32_t blockSize)
{
    arm_biquad_cascade_df2T_f32(&tape_freak->BASS_Instance, input, output, blockSize);
}

ITCM_AREA_CODE static void apply_tape_freak_treble(TAPE_FREAK_Effect_TypeDef *tape_freak,float32_t* input, float32_t* output, uint32_t blockSize)
{
    arm_biquad_cascade_df2T_f32(&tape_freak->TREBLE_Instance, input, output, blockSize);
}

ITCM_AREA_CODE static float32_t apply_tape_freak_bitcrusher(float32_t sample)
{
    static uint8_t counter = 0;
    static const uint8_t downsample_factor = 2;
    static const uint8_t bit_depth = 12;
    static float32_t last_sample = 0.0f;

    if (counter++ % downsample_factor == 0) {
        int32_t q = 1 << (16 - bit_depth);
        sample = ((int32_t)(sample * 32767.0f) / q) * q / 32768.0f;
        last_sample = sample;
    }
    return last_sample;
}

ITCM_AREA_CODE static void tape_freak_process(TAPE_FREAK_Effect_TypeDef *tape_freak,float32_t* input, q15_t* output, uint32_t blockSize)
{

    q31_t	result;
    for (int i = 0; i < blockSize; i++) {
    	tape_freak->temp1[i] = input[i] / 32768.0f;
    }

    for (int i = 0; i < blockSize; i++) {
    	tape_freak->temp2[i] = tape_freak->temp1[i] * tape_freak->preamp_gain;
    }

    for (int i = 0; i < blockSize; i++) {
    	tape_freak->temp1[i] = tanhf(tape_freak->temp2[i]);
    }

    if ((tape_freak->flags & TAPE_FREAK_FLAGS_BITCRUSHER_ENABLED ) == TAPE_FREAK_FLAGS_BITCRUSHER_ENABLED)
    for (int i = 0; i < blockSize; i++)
    	tape_freak->temp2[i] = apply_tape_freak_bitcrusher(tape_freak->temp1[i]);
    else
    	memcpy(tape_freak->temp2, tape_freak->temp1, TAPE_FREAK_BUFFER_SIZE);

    if ((tape_freak->flags & TAPE_FREAK_FLAGS_TONE_CONTROL_ENABLED ) == TAPE_FREAK_FLAGS_TONE_CONTROL_ENABLED)
    {
        apply_tape_freak_bass(tape_freak,tape_freak->temp2, tape_freak->temp1, blockSize);
        apply_tape_freak_treble(tape_freak,tape_freak->temp1, tape_freak->temp2, blockSize);
    }
    else
        memcpy(tape_freak->temp2, tape_freak->temp1,TAPE_FREAK_BUFFER_SIZE);

    if ((tape_freak->flags & TAPE_FREAK_FLAGS_LOFI_FILTER_ENABLED ) == TAPE_FREAK_FLAGS_LOFI_FILTER_ENABLED)
        apply_tape_freak_lpf(tape_freak,tape_freak->temp2, tape_freak->temp1, blockSize);
    else
    	memcpy(tape_freak->temp1, tape_freak->temp2, TAPE_FREAK_BUFFER_SIZE);

    for (int i = 0; i < blockSize; i++) {
    	result = (q31_t )(tape_freak->temp1[i] * 32768.0f);
        if (result > 32767.0f)
        	result = 32767.0f;
        if (result < -32768.0f)
        	result = -32768.0f;
        output[i] = (q15_t )result;
    }
}

ITCM_AREA_CODE void Effect_Tape_Freak_Init(uint32_t *effect_s)
{
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
TAPE_FREAK_Effect_TypeDef *tape_freak = (TAPE_FREAK_Effect_TypeDef *)effect->private_data;

	tape_freak->biquadCoeffsLP[0] = 0.09944f;
	tape_freak->biquadCoeffsLP[1] = 0.19888f;
	tape_freak->biquadCoeffsLP[2] = 0.09944f;
	tape_freak->biquadCoeffsLP[3] = 1.0f;
	tape_freak->biquadCoeffsLP[4] = -0.80245f;

	tape_freak->biquadCoeffsBass[0] = 1.0352f;
	tape_freak->biquadCoeffsBass[1] = -1.9021f;
	tape_freak->biquadCoeffsBass[2] = 0.8830f;
	tape_freak->biquadCoeffsBass[3] = 1.0f;
	tape_freak->biquadCoeffsBass[4] = -1.8131f;

	tape_freak->biquadCoeffsTreble[0] = 1.0352f;
	tape_freak->biquadCoeffsTreble[1] = -1.9021f;
	tape_freak->biquadCoeffsTreble[2] = 0.8830f;
	tape_freak->biquadCoeffsTreble[3] = 1.0f;
	tape_freak->biquadCoeffsTreble[4] = -1.8131f;

	tape_freak->tone_gain_bass = 1.0f;
	tape_freak->tone_gain_treble = 1.0f;
	tape_freak->preamp_gain = 1.0f;

    arm_biquad_cascade_df2T_init_f32(&tape_freak->LPF_Instance, 1, tape_freak->biquadCoeffsLP, tape_freak->lpfState);
    arm_biquad_cascade_df2T_init_f32(&tape_freak->BASS_Instance, 1, tape_freak->biquadCoeffsBass, tape_freak->bassState);
    arm_biquad_cascade_df2T_init_f32(&tape_freak->TREBLE_Instance, 1, tape_freak->biquadCoeffsTreble, tape_freak->trebleState);
    tape_freak->status |= SOUND_EFFECT_INITIALIZED;
    effect->status |= SOUND_EFFECT_INITIALIZED;
}

ITCM_AREA_CODE void Effect_Tape_Freak(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
TAPE_FREAK_Effect_TypeDef *tape_freak = (TAPE_FREAK_Effect_TypeDef *)effect->private_data;

	if ((( tape_freak->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( tape_freak == NULL ))
		return;
	if (( tape_freak->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
		tape_freak_process(tape_freak,(float *)effect->in_buf,effect->out_buf + start_sample,TAPE_FREAK_BUFFER_SIZE);
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			effect->out_buf[i + start_sample]  = effect->in_buf[i];
	}
}
#endif // #ifdef SOUND_ENABLED
