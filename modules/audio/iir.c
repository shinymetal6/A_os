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

#include "effects.h"

extern	EffectsTypeDef	Effect[MAX_EFFECTS];

//4 delayed samples per biquad
OSCILLATORS_RAM	float iir_state [4];
OSCILLATORS_RAM	float iir_coeffs [5];

OSCILLATORS_RAM	arm_biquad_casd_df1_inst_f32 iirsettings;

ITCM_AREA_CODE void iir_set_params(uint8_t Type, uint16_t Frequency, float iir_Q)
{
//good value for iir_Q is 0.7F;
float  norm;
float iir_K = tan(PI*(float)Frequency/(float )SAMPLE_FREQUENCY);

	Effect[IIR_EFFECT_ID].parameter[0] = (float )Type;
	Effect[IIR_EFFECT_ID].parameter[1] = (float )Frequency;
	Effect[IIR_EFFECT_ID].parameter[2] = (float )iir_Q;

	norm = 1.0F / (1.0F + iir_K / iir_Q + iir_K * iir_K);
	switch(Type)
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

ITCM_AREA_CODE void Do_iir(int16_t *filter_in , int16_t *filter_out)
{
uint32_t	i;

	if ( (Effect[IIR_EFFECT_ID].effect_enabled & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for (i=0; i<HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
			iir_buf_in[i] = (float)filter_in[i];
		arm_biquad_cascade_df1_f32 (&iirsettings, (float32_t *)iir_buf_in, (float32_t * )iir_buf_out,HALF_NUMBER_OF_AUDIO_SAMPLES);
		for (i=0; i<HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
			filter_out[i] =	(int16_t)(iir_buf_out[i] );
	}
	else
	{
		for (i=0; i<HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
			filter_out[i] = filter_in[i];
	}

}

void Iir_init(uint8_t Type, uint16_t Frequency, float iir_Q)
{
	Effect[IIR_EFFECT_ID].parameter[0] = (float )Type;
	Effect[IIR_EFFECT_ID].parameter[1] = (float )Frequency;
	Effect[IIR_EFFECT_ID].parameter[2] = (float )iir_Q;
	Effect[IIR_EFFECT_ID].num_params = 3;
	sprintf(Effect[IIR_EFFECT_ID].effect_name,"Iir");
	switch ( Type )
	{
	case	IIR_HIGH_PASS :sprintf(Effect[IIR_EFFECT_ID].effect_param[0],"IIR High Pass"); break;
	case	IIR_LOW_PASS  :sprintf(Effect[IIR_EFFECT_ID].effect_param[0],"IIR Low Pass"); break;
	case	IIR_BAND_PASS :sprintf(Effect[IIR_EFFECT_ID].effect_param[0],"IIR Band Pass"); break;
	case	IIR_NOTCH     :sprintf(Effect[IIR_EFFECT_ID].effect_param[0],"IIR Notch"); break;
	}

	sprintf(Effect[IIR_EFFECT_ID].effect_param[1],"Frequency");
	sprintf(Effect[IIR_EFFECT_ID].effect_param[1],"Depth");
	Effect[IIR_EFFECT_ID].do_effect =  Do_iir;
	Effect[IIR_EFFECT_ID].effect_enabled = 0;
	iir_set_params(Type, Frequency, iir_Q);
	arm_biquad_cascade_df1_init_f32 ( &iirsettings, 1, &iir_coeffs[0], &iir_state[0]);
}

void Iir_enable(void)
{
	Effect[IIR_EFFECT_ID].effect_enabled |= EFFECT_ENABLED;
}

void Iir_disable(void)
{
	Effect[IIR_EFFECT_ID].effect_enabled &= ~EFFECT_ENABLED;
}

#endif
