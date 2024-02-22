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

#include "effect.h"
#include "wah.h"
#include "arm_math.h"

// setup parameters
#define WAHWAH_CENTRE_IDX  0
#define WAHWAH_RATE_IDX    1
#define WAHWAH_DEPTH_IDX   2
static float parameterValues[3];
static Parameter paramDepth       = {"Depth[%]       ", 10.f, 0.0f, 100.f};
static Parameter paramRate        = {"Rate[Hz]       ", 1.0f, 1.0f,  10.0f};
static Parameter paramCentreFreq  = {"Cutoff[Hz]", 100.0f, 100.0f,  4000.0f}; // 100 to 4000 hz
static Parameter parameters[3];
Effect wahwah;

// bandpass filter coeff's.
static float bp_a0, bp_a1, bp_a2, bp_b0, bp_b1, bp_b2;

// bandpass previous samples.
static float bp_x1, bp_x2, bp_y1, bp_y2;

static float currentCutoff = 440.0f;
static float qFactor = 2.3f;

static void new_bandpass()
{
    float omega = 2 * PI * currentCutoff / SAMPLE_FREQUENCY;
    float cosomega = arm_cos_f32(omega);
    float alpha = arm_sin_f32(omega) / (2 * qFactor);

    bp_b0 = alpha;
    bp_b1 = 0;
    bp_b2 = -alpha;
    bp_a0 = 1 + alpha;
    bp_a1 = -2 * cosomega;
    bp_a2 = 1 - alpha;
}

static int16_t apply_bandpass(int16_t inSample)
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

void Do_Wah(uint16_t* inputData, uint16_t* outputData, uint32_t offset)
{
	if(wahwah.on)
	{
		static float phase = 0;
		float lfoFreq = parameterValues[WAHWAH_RATE_IDX];
		float lfoDepth = parameterValues[WAHWAH_DEPTH_IDX]/100.0f;
		float centreFreq = parameterValues[WAHWAH_CENTRE_IDX];

		for(int i = offset; i < offset+(HALF_NUMBER_OF_AUDIO_SAMPLES); i++)
		{
			// update phase
			phase = fmodf((phase + lfoFreq / 44100), 1);

			// get triangle lfo sample
			float lfoSample = phase < 0.5 ? phase * 4 - 1 : 3 - 4 * phase;

			// modulate bandpass cutoff
			currentCutoff = (float)((lfoSample * lfoDepth * centreFreq) + centreFreq);

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
}

void Wah_init(void)
{
	// init params
	parameters[WAHWAH_CENTRE_IDX] = paramCentreFreq;
	parameters[WAHWAH_RATE_IDX] = paramRate;
	parameters[WAHWAH_DEPTH_IDX] = paramDepth;

	parameterValues[WAHWAH_CENTRE_IDX] = 550.0f;
	parameterValues[WAHWAH_RATE_IDX]   = 4.0f;
	parameterValues[WAHWAH_DEPTH_IDX]  = 70.0f;

	// init effect object
	strcpy( wahwah.name, "Wah" );
	wahwah.on = 0;
	wahwah.currentParam = 0;
	wahwah.paramNum = 3;
	wahwah.parameters = parameters;
	wahwah.processBuffer = Do_Wah;
	wahwah.paramValues = parameterValues;

	new_bandpass();
}

#endif
