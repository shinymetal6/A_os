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
 * moog1_filter.c
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

extern	EffectsTypeDef	Effect[MAX_EFFECTS];

MoogParametersTypeDef	Moog1Parameters;

#define	MOOG1_CUTOFF		0
#define	MOOG1_RESONANCE		1

void Set_Filter1_Coefficients(void)
{
/*    Set coefficients given frequency & resonance [0.0...1.0] */
	Moog1Parameters.q = 1.0F - Effect[MOOG1_F_EFFECT_ID].parameter[MOOG1_CUTOFF] ;
	Moog1Parameters.p = Effect[MOOG1_F_EFFECT_ID].parameter[MOOG1_CUTOFF]  + 0.8F * Effect[MOOG1_F_EFFECT_ID].parameter[MOOG1_CUTOFF]  * Moog1Parameters.q;
	Moog1Parameters.f = Moog1Parameters.p + Moog1Parameters.p - 1.0F;
	Moog1Parameters.q = Effect[MOOG1_F_EFFECT_ID].parameter[MOOG1_RESONANCE] * (1.0F + 0.5f * Moog1Parameters.q * (1.0F - Moog1Parameters.q + 5.6F * Moog1Parameters.q * Moog1Parameters.q));
}

void Clear_Moog1_data(void)
{
	Moog1Parameters.b0=Moog1Parameters.b1=Moog1Parameters.b2=Moog1Parameters.b3=Moog1Parameters.b4=0;
	Moog1Parameters.in1=Moog1Parameters.in2=Moog1Parameters.in3=Moog1Parameters.in4=0;
	Moog1Parameters.out1=Moog1Parameters.out2=Moog1Parameters.out3=Moog1Parameters.out4=0;
}

float process_moog1_filter(float in)
{
float t1, t2;              //temporary buffers
	in -= Moog1Parameters.q * Moog1Parameters.b4;                          //feedback
	t1 = Moog1Parameters.b1;
	Moog1Parameters.b1 = (in + Moog1Parameters.b0) * Moog1Parameters.p - Moog1Parameters.b1 * Moog1Parameters.f;
	t2 = Moog1Parameters.b2;
	Moog1Parameters.b2 = (Moog1Parameters.b1 + t1) * Moog1Parameters.p - Moog1Parameters.b2 * Moog1Parameters.f;
	t1 = Moog1Parameters.b3;
	Moog1Parameters.b3 = (Moog1Parameters.b2 + t2) * Moog1Parameters.p - Moog1Parameters.b3 * Moog1Parameters.f;
	Moog1Parameters.b4 = (Moog1Parameters.b3 + t1) * Moog1Parameters.p - Moog1Parameters.b4 * Moog1Parameters.f;
	Moog1Parameters.b4 = Moog1Parameters.b4 - Moog1Parameters.b4 * Moog1Parameters.b4 * Moog1Parameters.b4 * 0.166667F;    //clipping
	Moog1Parameters.b0 = in;
	/*
	if (( SystemFlags.afx_flags & AFX_CONTROL_BP ) == AFX_CONTROL_BP)
		return 3.0f * (Moog1Parameters.b3 - Moog1Parameters.b4);
	if (( SystemFlags.afx_flags & AFX_CONTROL_LP ) == AFX_CONTROL_LP) // in -= q*(b4 - in)
		return Moog1Parameters.b4;
	if (( SystemFlags.afx_flags & AFX_CONTROL_HP ) == AFX_CONTROL_HP)
		return (in - 3.0f * (Moog1Parameters.b3 - Moog1Parameters.b4))-Moog1Parameters.b4;
		*/
	return 0;
	// Lowpass  output:  b4
	// Highpass output:  in - b4; or (in - 3.0f * (b3 - b4))-b4;
	// Bandpass output:  3.0f * (b3 - b4);
}

ITCM_AREA_CODE void Do_Moog1(int16_t* inputData, int16_t* outputData)
{
	uint16_t	i;
	if ( (Effect[MOOG1_F_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			outputData[i] = process_moog1_filter ((float) inputData[i]);
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Moog1_init(float Cutoff,float Resonance)
{
	Effect[MOOG1_F_EFFECT_ID].parameter[MOOG1_CUTOFF] = Cutoff;
	Effect[MOOG1_F_EFFECT_ID].parameter[MOOG1_RESONANCE] = Resonance;
	Effect[MOOG1_F_EFFECT_ID].num_params = 2;
	sprintf(Effect[MOOG1_F_EFFECT_ID].effect_name,"Moog2 Filter");
	sprintf(Effect[MOOG1_F_EFFECT_ID].effect_param[MOOG1_CUTOFF],"Cutoff");
	sprintf(Effect[MOOG1_F_EFFECT_ID].effect_param[MOOG1_RESONANCE],"Resonance");
	Effect[MOOG1_F_EFFECT_ID].do_effect =  Do_Moog1;
	Effect[MOOG1_F_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

void Moog1_enable(void)
{
	Effect[MOOG1_F_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Moog1_disable(void)
{
	Effect[MOOG1_F_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif
