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
 * iir_s.c
 *
 *  Created on: Feb 24, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effects.h"
#include "iir_s.h"

extern	EffectsTypeDef	Effect[MAX_EFFECTS];

OSCILLATORS_RAM	IIR_S_TypeDef	IIR_S_Data;

ITCM_AREA_CODE void Iir_S_configure(uint8_t Type, uint16_t Frequency, float iir_Q)
{
//good value for iir_Q is 0.707F;
float  norm;
float iir_K = tan(PI*(float)Frequency/(float )SAMPLE_FREQUENCY);

	norm = 1.0F / (1.0F + iir_K / iir_Q + iir_K * iir_K);
	switch(Type)
	{
	case	IIR_HIGH_PASS	:
		IIR_S_Data.a[0] = 1.0F*norm;
		IIR_S_Data.a[1] = -2.0F * IIR_S_Data.a[0];
		IIR_S_Data.a[2] = IIR_S_Data.a[0];
		IIR_S_Data.b[1] = -(2.0F * (iir_K*iir_K - 1.0F)*norm);
		IIR_S_Data.b[2] = -(1.0F - iir_K / iir_Q + iir_K*iir_K) * norm;
		break;
	case	IIR_LOW_PASS	:
		IIR_S_Data.a[0] = iir_K * iir_K * norm;
		IIR_S_Data.a[1] = 2.0F * IIR_S_Data.a[0];
		IIR_S_Data.a[2] = IIR_S_Data.a[0];
		IIR_S_Data.b[1] = -2.0F * (iir_K * iir_K - 1.0F) * norm;
		IIR_S_Data.b[2] = -(1.0F - iir_K / iir_Q + iir_K * iir_K) * norm;
		break;
	case	IIR_BAND_PASS	:
		IIR_S_Data.a[0] = iir_K /iir_Q * norm;
		IIR_S_Data.a[1] = 0.0f;
		IIR_S_Data.a[2] = -IIR_S_Data.a[0];
		IIR_S_Data.b[1] = 2.0F * (iir_K*iir_K - 1.0F) * norm;
		IIR_S_Data.b[2] = (1.0F - iir_K / iir_Q + iir_K*iir_K) * norm;
		break;
	case	IIR_NOTCH	:
        IIR_S_Data.a[0] = (1.0F + iir_K * iir_K) * norm;
        IIR_S_Data.a[1] = 2.0F * (iir_K * iir_K - 1) * norm;
        IIR_S_Data.a[2] = IIR_S_Data.a[0];
        IIR_S_Data.b[1] = IIR_S_Data.a[1];
        IIR_S_Data.b[2] = (1.0F - iir_K / iir_Q + iir_K * iir_K) * norm;
        break;
	}
}

ITCM_AREA_CODE int16_t Iir_s (int16_t inSample)
{
	float inSampleF = (float)inSample;
	float outSampleF =
			IIR_S_Data.a[0] * inSampleF
			+ IIR_S_Data.a[1] * IIR_S_Data.hp_in_z1
			+ IIR_S_Data.a[2] * IIR_S_Data.hp_in_z2
			- IIR_S_Data.b[1] * IIR_S_Data.hp_out_z1
			- IIR_S_Data.b[2] * IIR_S_Data.hp_out_z2;
	IIR_S_Data.hp_in_z2 = IIR_S_Data.hp_in_z1;
	IIR_S_Data.hp_in_z1 = inSampleF;
	IIR_S_Data.hp_out_z2 = IIR_S_Data.hp_out_z1;
	IIR_S_Data.hp_out_z1 = outSampleF;
	return (int16_t) outSampleF;
}

ITCM_AREA_CODE void Do_iir_s(int16_t *inputData, int16_t *outputData)
{
	if ( (Effect[IIR_S_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
		*outputData = Iir_s(*inputData);
	else
		*outputData = *inputData;
}

void Iir_S_init(uint8_t Type, uint16_t Frequency, float iir_Q)
{
	Effect[IIR_S_EFFECT_ID].parameter[0] = (float )Type;
	Effect[IIR_S_EFFECT_ID].parameter[1] = (float )Frequency;
	Effect[IIR_S_EFFECT_ID].parameter[2] = (float )iir_Q;
	Effect[IIR_S_EFFECT_ID].num_params = 3;
	sprintf(Effect[IIR_S_EFFECT_ID].effect_name,"Iir");
	switch ( Type )
	{
	case	IIR_HIGH_PASS :sprintf(Effect[IIR_S_EFFECT_ID].effect_param[0],"IIR High Pass");break;
	case	IIR_LOW_PASS  :sprintf(Effect[IIR_S_EFFECT_ID].effect_param[0],"IIR Low Pass"); break;
	case	IIR_BAND_PASS :sprintf(Effect[IIR_S_EFFECT_ID].effect_param[0],"IIR Band Pass");break;
	case	IIR_NOTCH     :sprintf(Effect[IIR_S_EFFECT_ID].effect_param[0],"IIR Notch");    break;
	}
	Iir_S_configure(Type,Frequency,iir_Q);

	sprintf(Effect[IIR_S_EFFECT_ID].effect_param[1],"Frequency");
	sprintf(Effect[IIR_S_EFFECT_ID].effect_param[1],"Depth");
	Effect[IIR_S_EFFECT_ID].apply_effect =  Do_iir_s;
	Effect[IIR_S_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;

	//300Hz high-pass, 48k , params from https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3/
	/*
	IIR_S_Data.a[0] = 0.9726136465544695f;
	IIR_S_Data.a[1] = -1.945227293108939f;
	IIR_S_Data.a[2] = 0.9726136465544695f;
	IIR_S_Data.b[1] = -1.9444771540706607f;
	IIR_S_Data.b[2] = 0.9459774321472173f;
	*/
}

void Iir_S_enable(void)
{
	Effect[IIR_S_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Iir_S_disable(void)
{
	Effect[IIR_S_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif
