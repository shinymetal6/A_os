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
 *  Created on: Feb 19, 2024
 *      Author: fil
 *      Stolen from https://github.com/YetAnotherElectronicsChannel
 *
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "iir.h"
#include "arm_math.h"

//4 delayed samples per biquad
OSCILLATORS_RAM	float iir_state [4];
OSCILLATORS_RAM	float iir_coeffs [5];

OSCILLATORS_RAM	arm_biquad_casd_df1_inst_f32 iirsettings;
OSCILLATORS_RAM	float iir_PI = 3.14159265359F;

ITCM_AREA_CODE void iir_set_params(uint8_t type, uint16_t frequency, float iir_Q)
{
float  norm;
float iir_K = tan(iir_PI*(float)frequency/(float )SAMPLE_FREQUENCY);
//float iir_Q = 0.7F;

	norm = 1.0F / (1.0F + iir_K / iir_Q + iir_K * iir_K);
	switch(type)
	{
	case	IIR_HIGH_PASS	:
		iir_coeffs[IIR_COEFF_A0] = 1.0F*norm;
		iir_coeffs[IIR_COEFF_A1] = -2.0F * iir_coeffs[IIR_COEFF_A0];
		iir_coeffs[IIR_COEFF_A2] = iir_coeffs[IIR_COEFF_A0];
		iir_coeffs[IIR_COEFF_B1] = -(2.0F * (iir_K*iir_K - 1.0F)*norm);
		iir_coeffs[IIR_COEFF_B2] = -(1.0F - iir_K / iir_Q + iir_K*iir_K) * norm;
		break;
	case	IIR_LOW_PASS	:
		iir_coeffs[IIR_COEFF_A0] = iir_K * iir_K * norm;
		iir_coeffs[IIR_COEFF_A1] = 2.0F * iir_coeffs[IIR_COEFF_A0];
		iir_coeffs[IIR_COEFF_A2] = iir_coeffs[IIR_COEFF_A0];
		iir_coeffs[IIR_COEFF_B1] = -2.0F * (iir_K * iir_K - 1.0F) * norm;
		iir_coeffs[IIR_COEFF_B2] = -(1.0F - iir_K / iir_Q + iir_K * iir_K) * norm;
		break;
	case	IIR_BAND_PASS	:
		iir_coeffs[IIR_COEFF_A0] = iir_K /iir_Q * norm;
		iir_coeffs[IIR_COEFF_A1] = 0.0f;
		iir_coeffs[IIR_COEFF_A2] = -iir_coeffs[IIR_COEFF_A0];
		iir_coeffs[IIR_COEFF_B1] = 2.0F * (iir_K*iir_K - 1.0F) * norm;
		iir_coeffs[IIR_COEFF_B2] = (1.0F - iir_K / iir_Q + iir_K*iir_K) * norm;
		break;
	case	IIR_NOTCH	:
        iir_coeffs[IIR_COEFF_A0] = (1.0F + iir_K * iir_K) * norm;
        iir_coeffs[IIR_COEFF_A1] = 2.0F * (iir_K * iir_K - 1) * norm;
        iir_coeffs[IIR_COEFF_A2] = iir_coeffs[IIR_COEFF_A0];
        iir_coeffs[IIR_COEFF_B1] = iir_coeffs[IIR_COEFF_A1];
        iir_coeffs[IIR_COEFF_B2] = (1.0F - iir_K / iir_Q + iir_K * iir_K) * norm;
        break;
	}
}

OSCILLATORS_RAM	float iir_buf_in  [HALF_NUMBER_OF_AUDIO_SAMPLES];
OSCILLATORS_RAM	float iir_buf_out [HALF_NUMBER_OF_AUDIO_SAMPLES];

ITCM_AREA_CODE void iir(int16_t *filter_out , int16_t *filter_in)
{
uint32_t	i;

	for (i=0; i<HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
		iir_buf_in[i] = (float)filter_in[i];
	arm_biquad_cascade_df1_f32 (&iirsettings, (float32_t *)iir_buf_in, (float32_t * )iir_buf_out,HALF_NUMBER_OF_AUDIO_SAMPLES);
	for (i=0; i<HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
		filter_out[i] =	(int16_t)(iir_buf_out[i] );
}

void iir_init(void)
{
	arm_biquad_cascade_df1_init_f32 ( &iirsettings, 1, &iir_coeffs[0], &iir_state[0]);
}

#endif
