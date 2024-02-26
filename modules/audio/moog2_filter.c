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
 * moog2_filter.c
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

MoogParametersTypeDef	Moog2Parameters;

#define	MOOG1_CUTOFF		0
#define	MOOG1_RESONANCE		1

void Set_Filter2_Coefficients(void)
{
	Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF]  *= 1.16F;
	Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_RESONANCE] *= (4.0F * (1.0F - 0.15F * Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF]  * Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] ));
	Moog2Parameters.filter2_constant01 = Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] *Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] *Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] *Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] *0.35013F;;
}

void Clear_Moog2_data(void)
{
	Moog2Parameters.b0=Moog2Parameters.b1=Moog2Parameters.b2=Moog2Parameters.b3=Moog2Parameters.b4=0;
	Moog2Parameters.in1=Moog2Parameters.in2=Moog2Parameters.in3=Moog2Parameters.in4=0;
	Moog2Parameters.out1=Moog2Parameters.out2=Moog2Parameters.out3=Moog2Parameters.out4=0;
}

float process_moog2_filter(float input)
{
	input -= Moog2Parameters.out4 * Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_RESONANCE];
	input *= Moog2Parameters.filter2_constant01; //0.35013 * (f*f)*(f*f);
	Moog2Parameters.out1 = input + 0.3F * Moog2Parameters.in1 + (1.0F - Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] ) * Moog2Parameters.out1; // Pole 1
	Moog2Parameters.in1 = input;
	Moog2Parameters.out2 = Moog2Parameters.out1 + 0.3F * Moog2Parameters.in2 + (1 - Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] ) * Moog2Parameters.out2; // Pole 2
	Moog2Parameters.in2  = Moog2Parameters.out1;
	Moog2Parameters.out3 = Moog2Parameters.out2 + 0.3F * Moog2Parameters.in3 + (1 - Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] ) * Moog2Parameters.out3; // Pole 3
	Moog2Parameters.in3  = Moog2Parameters.out2;
	Moog2Parameters.out4 = Moog2Parameters.out3 + 0.3F * Moog2Parameters.in4 + (1 - Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] ) * Moog2Parameters.out4; // Pole 4
	Moog2Parameters.in4 = Moog2Parameters.out3;
	/*
	if (( SystemFlags.afx_flags & AFX_CONTROL_BP ) == AFX_CONTROL_BP)
		return 3.0f * (Moog2Parameters.out3 - Moog2Parameters.out4);
	if (( SystemFlags.afx_flags & AFX_CONTROL_LP ) == AFX_CONTROL_LP)
		return Moog2Parameters.out4;
	if (( SystemFlags.afx_flags & AFX_CONTROL_HP ) == AFX_CONTROL_HP)
		return input - Moog2Parameters.out4;
		*/
	return 0;
}

ITCM_AREA_CODE void Do_Moog2(int16_t* inputData, int16_t* outputData)
{
	uint16_t	i;
	if ( (Effect[MOOG2_F_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			outputData[i] = process_moog2_filter ((float) inputData[i]);
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Moog2_init(float Cutoff,float Resonance)
{
	Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_CUTOFF] = Cutoff;
	Effect[MOOG2_F_EFFECT_ID].parameter[MOOG1_RESONANCE] = Resonance;
	Effect[MOOG2_F_EFFECT_ID].num_params = 2;
	sprintf(Effect[MOOG2_F_EFFECT_ID].effect_name,"Moog1 Filter");
	sprintf(Effect[MOOG2_F_EFFECT_ID].effect_param[MOOG1_CUTOFF],"Cutoff");
	sprintf(Effect[MOOG2_F_EFFECT_ID].effect_param[MOOG1_RESONANCE],"Resonance");
	Effect[MOOG2_F_EFFECT_ID].apply_effect =  Do_Moog2;
	Effect[MOOG2_F_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

void Moog2_enable(void)
{
	Effect[MOOG2_F_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Moog2_disable(void)
{
	Effect[MOOG2_F_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif

