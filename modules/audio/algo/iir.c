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

extern	BlockEffectsTypeDef	BlockEffect[MAX_BLOCK_EFFECTS];

//4 delayed samples per biquad
AUDIO_FAST_RAM	float iir_state [4];
AUDIO_FAST_RAM	IIR_TypeDef	IIR_Data;
AUDIO_FAST_RAM	float arm_iir_coeffs [5];
AUDIO_FAST_RAM	arm_biquad_casd_df1_inst_f32 iirsettings;

ITCM_AREA_CODE void Iir_configure(uint8_t Type, uint16_t Frequency, float iir_Q)
{
//good value for iir_Q is 0.707F;
float  norm;
float iir_K = tan(PI*(float)Frequency/(float )SAMPLE_FREQUENCY);

	BlockEffect[IIR_EFFECT_ID].parameter[0] = (float )Type;
	BlockEffect[IIR_EFFECT_ID].parameter[1] = (float )Frequency;
	BlockEffect[IIR_EFFECT_ID].parameter[2] = (float )iir_Q;

	norm = 1.0F / (1.0F + iir_K / iir_Q + iir_K * iir_K);
	switch(Type)
	{
	case	IIR_HIGH_PASS	:
		IIR_Data.a[0] = 1.0F*norm;
		IIR_Data.a[1] = -2.0F * IIR_Data.a[0];
		IIR_Data.a[2] = IIR_Data.a[0];
		IIR_Data.b[1] = -(2.0F * (iir_K*iir_K - 1.0F)*norm);
		IIR_Data.b[2] = -(1.0F - iir_K / iir_Q + iir_K*iir_K) * norm;
		break;
	case	IIR_LOW_PASS	:
		IIR_Data.a[0] = iir_K * iir_K * norm;
		IIR_Data.a[1] = 2.0F * IIR_Data.a[0];
		IIR_Data.a[2] = IIR_Data.a[0];
		IIR_Data.b[1] = -2.0F * (iir_K * iir_K - 1.0F) * norm;
		IIR_Data.b[2] = -(1.0F - iir_K / iir_Q + iir_K * iir_K) * norm;
		break;
	case	IIR_BAND_PASS	:
        IIR_Data.a[0] = iir_K / iir_Q * norm;
        IIR_Data.a[1] = 0.0f;
        IIR_Data.a[2] = -IIR_Data.a[0];
        IIR_Data.b[1] = 2.0f * (iir_K * iir_K - 1.0f) * norm;
        IIR_Data.b[2] = (1.0f - iir_K / iir_Q + iir_K * iir_K) * norm;
        break;

		break;
	case	IIR_NOTCH	:
        IIR_Data.a[0] = (1.0F + iir_K * iir_K) * norm;
        IIR_Data.a[1] = 2.0F * (iir_K * iir_K - 1) * norm;
        IIR_Data.a[2] = IIR_Data.a[0];
        IIR_Data.b[1] = IIR_Data.a[1];
        IIR_Data.b[2] = (1.0F - iir_K / iir_Q + iir_K * iir_K) * norm;
        break;
	}
	arm_iir_coeffs[0] = IIR_Data.a[0];
	arm_iir_coeffs[1] = IIR_Data.a[1];
	arm_iir_coeffs[2] = IIR_Data.a[2];
	arm_iir_coeffs[3] = IIR_Data.b[1];
	arm_iir_coeffs[4] = IIR_Data.b[2];
}

AUDIO_FAST_RAM	float iir_buf_in  [HALF_NUMBER_OF_AUDIO_SAMPLES];
AUDIO_FAST_RAM	float iir_buf_out [HALF_NUMBER_OF_AUDIO_SAMPLES];

extern	void A_os_arm_biquad_cascade( const arm_biquad_casd_df1_inst_f32 * S, float32_t * pSrc, float32_t * pDst, uint32_t blockSize);

ITCM_AREA_CODE void Do_iir(int16_t *filter_in , int16_t *filter_out)
{
uint32_t	i;

	if ( (BlockEffect[IIR_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for (i=0; i<HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
			iir_buf_in[i] = (float)filter_in[i];
		arm_biquad_cascade_df1_f32 (&iirsettings, (float32_t *)iir_buf_in, (float32_t * )iir_buf_out,HALF_NUMBER_OF_AUDIO_SAMPLES);
//		A_os_arm_biquad_cascade (&iirsettings, (float32_t *)iir_buf_in, (float32_t * )iir_buf_out,HALF_NUMBER_OF_AUDIO_SAMPLES);
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
	BlockEffect[IIR_EFFECT_ID].parameter[0] = (float )Type;
	BlockEffect[IIR_EFFECT_ID].parameter[1] = (float )Frequency;
	BlockEffect[IIR_EFFECT_ID].parameter[2] = (float )iir_Q;
	BlockEffect[IIR_EFFECT_ID].num_params = 3;
	sprintf(BlockEffect[IIR_EFFECT_ID].effect_name,"Iir");
	switch ( Type )
	{
	case	IIR_HIGH_PASS :sprintf(BlockEffect[IIR_EFFECT_ID].effect_param[0],"IIR High Pass"); break;
	case	IIR_LOW_PASS  :sprintf(BlockEffect[IIR_EFFECT_ID].effect_param[0],"IIR Low Pass"); break;
	case	IIR_BAND_PASS :sprintf(BlockEffect[IIR_EFFECT_ID].effect_param[0],"IIR Band Pass"); break;
	case	IIR_NOTCH     :sprintf(BlockEffect[IIR_EFFECT_ID].effect_param[0],"IIR Notch"); break;
	}

	sprintf(BlockEffect[IIR_EFFECT_ID].effect_param[1],"Frequency");
	sprintf(BlockEffect[IIR_EFFECT_ID].effect_param[1],"Depth");
	BlockEffect[IIR_EFFECT_ID].apply_effect =  Do_iir;
	BlockEffect[IIR_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
	Iir_configure(Type, Frequency, iir_Q);
	arm_biquad_cascade_df1_init_f32 ( &iirsettings, 1, arm_iir_coeffs, &iir_state[0]);
}

void Iir_enable(void)
{
	BlockEffect[IIR_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Iir_disable(void)
{
	BlockEffect[IIR_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif
