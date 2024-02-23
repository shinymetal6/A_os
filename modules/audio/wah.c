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
 * wah.c
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effects.h"

// bandpass filter coeff's.
static float bp_a0, bp_a1, bp_a2, bp_b0, bp_b1, bp_b2;

// bandpass previous samples.
static float bp_x1, bp_x2, bp_y1, bp_y2;

static float currentCenterFrequency = 440.0f;
static float qFactor = 2.3f;

OSCILLATORS_RAM	WahTypeDef	WahData;

ITCM_AREA_CODE static void new_bandpass()
{
    float omega = 2 * PI * currentCenterFrequency / SAMPLE_FREQUENCY;
    float cosomega = arm_cos_f32(omega);
    float alpha = arm_sin_f32(omega) / (2 * qFactor);

    bp_b0 = alpha;
    bp_b1 = 0;
    bp_b2 = -alpha;
    bp_a0 = 1 + alpha;
    bp_a1 = -2 * cosomega;
    bp_a2 = 1 - alpha;
}

ITCM_AREA_CODE static int16_t apply_bandpass(int16_t inSample)
{
	float x0 = (float)inSample;
	float result =
		(bp_b0 / bp_a0) * x0 +
		(bp_b1 / bp_a0) * bp_x1 +
		(bp_b2 / bp_a0) * bp_x2 -
		(bp_a1 / bp_a0) * bp_y1 -
		(bp_a2 / bp_a0) * bp_y2;

	// shift x1 to x2, sample to x1
	// shift y1 to y2, result to y1
	// simulate delay!!
	bp_x2 = bp_x1;
	bp_x1 = x0;
	bp_y2 = bp_y1;
	bp_y1 = result;

	return (int16_t)result;
}

ITCM_AREA_CODE void Do_Wah(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;

	if ( (Effect[WAH_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		static float phase = 0;
		float lfoFreq = Effect[WAH_EFFECT_ID].parameter[1];
		float lfoDepth = Effect[WAH_EFFECT_ID].parameter[2]/100.0f;
		float centreFreq =Effect[WAH_EFFECT_ID].parameter[0];

		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			// update phase
			phase = fmodf((phase + lfoFreq / 44100), 1);

			// get triangle lfo sample
			float lfoSample = phase < 0.5 ? phase * 4 - 1 : 3 - 4 * phase;

			// modulate bandpass cutoff
			currentCenterFrequency = (float)((lfoSample * lfoDepth * centreFreq) + centreFreq);

			// update bandpass filter
			new_bandpass();

			// get current sample
			int16_t sample = (int16_t) outputData[i];

			// apply bandpass filter to current sample
			sample = apply_bandpass(sample);

			// send to output
			outputData[i] = (uint16_t) (sample);
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Wah_init(uint32_t CenterFrequency,uint32_t Rate,uint32_t Depth)
{
/*
typical values
	parameter[0] = "CenterFrequency[Hz]", 100.0f, 100.0f,  4000.0f; // 100 to 4000 hz , typ 550hz
	parameter[1] = "Rate[Hz]       ", 1.0f, 1.0f,  10.0f // typ 70.0f;
	parameter[2] = "Depth[%]       ", 10.f, 0.0f, 100.f // typ 4.0f;
*/

	Effect[WAH_EFFECT_ID].parameter[0] = (float )CenterFrequency;
	Effect[WAH_EFFECT_ID].parameter[1] = (float )Rate;
	Effect[WAH_EFFECT_ID].parameter[2] = (float )Depth;
	Effect[WAH_EFFECT_ID].num_params = 3;
	sprintf(Effect[WAH_EFFECT_ID].effect_name,"Wah");
	sprintf(Effect[WAH_EFFECT_ID].effect_param[0],"CenterFrequency");
	sprintf(Effect[WAH_EFFECT_ID].effect_param[1],"Rate");
	sprintf(Effect[WAH_EFFECT_ID].effect_param[1],"Depth");
	Effect[WAH_EFFECT_ID].do_effect =  Do_Wah;
	Effect[WAH_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
	new_bandpass();
}

void Wah_enable(void)
{
	Effect[WAH_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Wah_disable(void)
{
	Effect[WAH_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}


#endif
