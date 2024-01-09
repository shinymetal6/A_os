/*
 * moog_vcf.c
 *
 *  Created on: Jun 30, 2021
 *      Author: fil
 */

#include "main.h"
#include "../audio_component.h"
AFXParametersTypeDef	AFXParameters;

void Set_Filter1_Coefficients(void)
{
/*    Set coefficients given frequency & resonance [0.0...1.0] */
	AFXParameters.q = 1.0F - AFXParameters.filterCutoff;
	AFXParameters.p = AFXParameters.filterCutoff + 0.8F * AFXParameters.filterCutoff * AFXParameters.q;
	AFXParameters.f = AFXParameters.p + AFXParameters.p - 1.0F;
	AFXParameters.q = AFXParameters.filterResonance * (1.0F + 0.5f * AFXParameters.q * (1.0F - AFXParameters.q + 5.6F * AFXParameters.q * AFXParameters.q));
}

void Set_Filter2_Coefficients(void)
{
	AFXParameters.filterCutoff *= 1.16F;
	AFXParameters.filterResonance *= (4.0F * (1.0F - 0.15F * AFXParameters.filterCutoff * AFXParameters.filterCutoff));
	AFXParameters.filter2_constant01 = AFXParameters.filterCutoff*AFXParameters.filterCutoff*AFXParameters.filterCutoff*AFXParameters.filterCutoff*0.35013F;;
}

void Clear_AFX_data(void)
{
	AFXParameters.b0=AFXParameters.b1=AFXParameters.b2=AFXParameters.b3=AFXParameters.b4=0;
	AFXParameters.in1=AFXParameters.in2=AFXParameters.in3=AFXParameters.in4=0;
	AFXParameters.out1=AFXParameters.out2=AFXParameters.out3=AFXParameters.out4=0;
}

float process_moog1_filter(float in)
{
float t1, t2;              //temporary buffers
	in -= AFXParameters.q * AFXParameters.b4;                          //feedback
	t1 = AFXParameters.b1;
	AFXParameters.b1 = (in + AFXParameters.b0) * AFXParameters.p - AFXParameters.b1 * AFXParameters.f;
	t2 = AFXParameters.b2;
	AFXParameters.b2 = (AFXParameters.b1 + t1) * AFXParameters.p - AFXParameters.b2 * AFXParameters.f;
	t1 = AFXParameters.b3;
	AFXParameters.b3 = (AFXParameters.b2 + t2) * AFXParameters.p - AFXParameters.b3 * AFXParameters.f;
	AFXParameters.b4 = (AFXParameters.b3 + t1) * AFXParameters.p - AFXParameters.b4 * AFXParameters.f;
	AFXParameters.b4 = AFXParameters.b4 - AFXParameters.b4 * AFXParameters.b4 * AFXParameters.b4 * 0.166667F;    //clipping
	AFXParameters.b0 = in;
	if (( SystemFlags.afx_flags & AFX_CONTROL_BP ) == AFX_CONTROL_BP)
		return 3.0f * (AFXParameters.b3 - AFXParameters.b4);
	if (( SystemFlags.afx_flags & AFX_CONTROL_LP ) == AFX_CONTROL_LP) // in -= q*(b4 - in)
		return AFXParameters.b4;
	if (( SystemFlags.afx_flags & AFX_CONTROL_HP ) == AFX_CONTROL_HP)
		return (in - 3.0f * (AFXParameters.b3 - AFXParameters.b4))-AFXParameters.b4;
	return 0;
	// Lowpass  output:  b4
	// Highpass output:  in - b4; or (in - 3.0f * (b3 - b4))-b4;
	// Bandpass output:  3.0f * (b3 - b4);
}

float process_moog2_filter(float input)
{
	input -= AFXParameters.out4 * AFXParameters.filterResonance;
	input *= AFXParameters.filter2_constant01; //0.35013 * (f*f)*(f*f);
	AFXParameters.out1 = input + 0.3F * AFXParameters.in1 + (1.0F - AFXParameters.filterCutoff) * AFXParameters.out1; // Pole 1
	AFXParameters.in1 = input;
	AFXParameters.out2 = AFXParameters.out1 + 0.3F * AFXParameters.in2 + (1 - AFXParameters.filterCutoff) * AFXParameters.out2; // Pole 2
	AFXParameters.in2  = AFXParameters.out1;
	AFXParameters.out3 = AFXParameters.out2 + 0.3F * AFXParameters.in3 + (1 - AFXParameters.filterCutoff) * AFXParameters.out3; // Pole 3
	AFXParameters.in3  = AFXParameters.out2;
	AFXParameters.out4 = AFXParameters.out3 + 0.3F * AFXParameters.in4 + (1 - AFXParameters.filterCutoff) * AFXParameters.out4; // Pole 4
	AFXParameters.in4 = AFXParameters.out3;
	if (( SystemFlags.afx_flags & AFX_CONTROL_BP ) == AFX_CONTROL_BP)
		return 3.0f * (AFXParameters.out3 - AFXParameters.out4);
	if (( SystemFlags.afx_flags & AFX_CONTROL_LP ) == AFX_CONTROL_LP)
		return AFXParameters.out4;
	if (( SystemFlags.afx_flags & AFX_CONTROL_HP ) == AFX_CONTROL_HP)
		return input - AFXParameters.out4;
	return 0;
}

void AFX( uint16_t *buffer_out,uint16_t *buffer_in )
{
uint16_t	i;

	if (( SystemFlags.afx_flags & AFX_ENABLED ) == 0)
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			*buffer_out++ = *buffer_in++;
	}
	else
	{
		if ((( SystemFlags.afxtype_flags & AFXTYPE_MOOG1) == AFXTYPE_MOOG1) || (( SystemFlags.afxtype_flags & AFXTYPE_MOOG2) == AFXTYPE_MOOG2))
		{
			if ( isnan(AFXParameters.b0) )
				AFXParameters.b0=AFXParameters.b1=AFXParameters.b2=AFXParameters.b3=AFXParameters.b4=0;
		}

		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			if (( SystemFlags.afxtype_flags & AFXTYPE_MOOG1) == AFXTYPE_MOOG1)
				*buffer_out++ = FLOAT_2_NORMALIZED_INT(process_moog1_filter(INT_2_NORMALIZED_FLOAT(*buffer_in++)));
			else if (( SystemFlags.afxtype_flags & AFXTYPE_MOOG2) == AFXTYPE_MOOG2)
				*buffer_out++ = FLOAT_2_NORMALIZED_INT(process_moog2_filter(INT_2_NORMALIZED_FLOAT(*buffer_in++)));
			else if (( SystemFlags.afxtype_flags & AFXTYPE_PHASER) == AFXTYPE_PHASER)
				*buffer_out++ = (uint16_t )process_Phaser((float )buffer_in[i] );
			else
				*buffer_out++ = *buffer_in++;
		}
	}
}
