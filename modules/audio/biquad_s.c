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
 * biquad_s.c
 *
 *  Created on: Feb 28, 2024
 *      Author: fil
 */

/*
 * adapted from https://www.musicdsp.org/en/latest/Filters/64-biquad-c-code.html Tom St Denis
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "biquad_s.h"
#include "math.h"
#include "effects.h"

OSCILLATORS_RAM	BIQUAD_TypeDef	BIQUAD_Data;

ITCM_AREA_CODE int16_t Do_Biquad_s(int16_t inputData)
{
	float result,sample = (float )inputData / BIQUAD_Data.resonance;

    /* compute result */
    result = BIQUAD_Data.a0 * sample + BIQUAD_Data.a1 * BIQUAD_Data.x1 + BIQUAD_Data.a2 * BIQUAD_Data.x2 - BIQUAD_Data.a3 * BIQUAD_Data.y1 - BIQUAD_Data.a4 * BIQUAD_Data.y2;

    /* shift x1 to x2, sample to x1 */
    BIQUAD_Data.x2 = BIQUAD_Data.x1;
    BIQUAD_Data.x1 = sample;

    /* shift y1 to y2, result to y1 */
    BIQUAD_Data.y2 = BIQUAD_Data.y1;
    BIQUAD_Data.y1 = result;

    return result;
}

ITCM_AREA_CODE void Biquad_s_configure(uint8_t Type, float Frequency, float iir_Q_BW, float resonance)
{
	float A,beta,omega, sn, cs, alpha;
	float a0, a1, a2, b0, b1, b2;
    /* setup variables */
	A = pow(10, iir_Q_BW / 40);
    omega = 2 * M_PI * Frequency /(float )SAMPLE_FREQUENCY;
    sn = sin(omega);
    cs = cos(omega);
    alpha = sn * sinh(M_LN2 /2 * iir_Q_BW * omega /sn);
    beta = sqrt(A + A);

    switch (Type)
    {
    case BIQUAD_LOW_PASS:
        b0 = (1 - cs) /2;
        b1 = 1 - cs;
        b2 = (1 - cs) /2;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case BIQUAD_HIGH_PASS:
        b0 = (1 + cs) /2;
        b1 = -(1 + cs);
        b2 = (1 + cs) /2;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case BIQUAD_BAND_PASS:
        b0 = alpha;
        b1 = 0;
        b2 = -alpha;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case BIQUAD_NOTCH:
        b0 = 1;
        b1 = -2 * cs;
        b2 = 1;
        a0 = 1 + alpha;
        a1 = -2 * cs;
        a2 = 1 - alpha;
        break;
    case BIQUAD_PEQ:
        b0 = 1 + (alpha * A);
        b1 = -2 * cs;
        b2 = 1 - (alpha * A);
        a0 = 1 + (alpha /A);
        a1 = -2 * cs;
        a2 = 1 - (alpha /A);
        break;
    case BIQUAD_LSH:
        b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
        b1 = 2 * A * ((A - 1) - (A + 1) * cs);
        b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
        a0 = (A + 1) + (A - 1) * cs + beta * sn;
        a1 = -2 * ((A - 1) + (A + 1) * cs);
        a2 = (A + 1) + (A - 1) * cs - beta * sn;
        break;
    case BIQUAD_HSH:
        b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
        b1 = -2 * A * ((A - 1) + (A + 1) * cs);
        b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
        a0 = (A + 1) - (A - 1) * cs + beta * sn;
        a1 = 2 * ((A - 1) - (A + 1) * cs);
        a2 = (A + 1) - (A - 1) * cs - beta * sn;
        break;
    default:
        return;
    }

    /* precompute the coefficients */
    BIQUAD_Data.a0 = b0 /a0;
    BIQUAD_Data.a1 = b1 /a0;
    BIQUAD_Data.a2 = b2 /a0;
    BIQUAD_Data.a3 = a1 /a0;
    BIQUAD_Data.a4 = a2 /a0;

    /* zero initial samples */
    BIQUAD_Data.x1 = BIQUAD_Data.x2 = 0;
    BIQUAD_Data.y1 = BIQUAD_Data.y2 = 0;
}

void Biquad_s_init(uint8_t Type, uint16_t Frequency, float iir_Q_BW, float resonance)
{
	SingleSampleEffect[BIQUAD_S_EFFECT_ID].parameter[0] = (float )Type / 100.0F;
	SingleSampleEffect[BIQUAD_S_EFFECT_ID].parameter[1] = (float )Frequency;
	SingleSampleEffect[BIQUAD_S_EFFECT_ID].parameter[2] = iir_Q_BW;
	SingleSampleEffect[BIQUAD_S_EFFECT_ID].num_params = 3;
	sprintf(SingleSampleEffect[BIQUAD_S_EFFECT_ID].effect_name,"Biquad");
	sprintf(SingleSampleEffect[BIQUAD_S_EFFECT_ID].effect_param[0],"Type");
	sprintf(SingleSampleEffect[BIQUAD_S_EFFECT_ID].effect_param[1],"Frequency");
	sprintf(SingleSampleEffect[BIQUAD_S_EFFECT_ID].effect_param[2],"Q / Bw");
	SingleSampleEffect[BIQUAD_S_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
	SingleSampleEffect[BIQUAD_S_EFFECT_ID].apply_effect =  Do_Biquad_s;

	BIQUAD_Data.resonance = resonance;
}


void Biquad_s_enable(void)
{
	SingleSampleEffect[BIQUAD_S_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Biquad_s_disable(void)
{
	SingleSampleEffect[BIQUAD_S_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}
#endif

