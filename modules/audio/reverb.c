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
 * reverb.c
 *
 *  Created on: Feb 21, 2024
 *      Author: fil
 *      Stolen from https://github.com/YetAnotherElectronicsChannel
 */
#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effects.h"

// Schroeder delays from 25k->96k interpolated
//*2 delay extension -> not more possible without external ram
#define l_CB0 3460*2
#define l_CB1 2988*2
#define l_CB2 3882*2
#define l_CB3 4312*2
#define l_AP0 480*2
#define l_AP1 161*2
#define l_AP2 46*2

OSCILLATORS_RAM ReverbTypeDef ReverbData;

//define buffer for comb- and allpassfilters
D2_BUFFER float cfbuf0[l_CB0], cfbuf1[l_CB1], cfbuf2[l_CB2], cfbuf3[l_CB3];
D2_BUFFER float apbuf0[l_AP0], apbuf1[l_AP1], apbuf2[l_AP2];


float Do_Comb0(float inSample)
{
	float readback = cfbuf0[ReverbData.cf0_p];
	float new = readback*ReverbData.cf0_g + inSample;
	cfbuf0[ReverbData.cf0_p] = new;
	ReverbData.cf0_p++;
	if (ReverbData.cf0_p==ReverbData.cf0_lim)
		ReverbData.cf0_p = 0;
	return readback;
}

float Do_Comb1(float inSample)
{
	float readback = cfbuf1[ReverbData.cf1_p];
	float new = readback*ReverbData.cf1_g + inSample;
	cfbuf1[ReverbData.cf1_p] = new;
	ReverbData.cf1_p++;
	if (ReverbData.cf1_p==ReverbData.cf1_lim)
		ReverbData.cf1_p = 0;
	return readback;
}

float Do_Comb2(float inSample)
{
	float readback = cfbuf2[ReverbData.cf2_p];
	float new = readback*ReverbData.cf2_g + inSample;
	cfbuf2[ReverbData.cf2_p] = new;
	ReverbData.cf2_p++;
	if (ReverbData.cf2_p==ReverbData.cf2_lim)
		ReverbData.cf2_p = 0;
	return readback;
}

float Do_Comb3(float inSample)
{
	float readback = cfbuf3[ReverbData.cf3_p];
	float new = readback*ReverbData.cf3_g + inSample;
	cfbuf3[ReverbData.cf3_p] = new;
	ReverbData.cf3_p++;
	if (ReverbData.cf3_p==ReverbData.cf3_lim)
		ReverbData.cf3_p = 0;
	return readback;
}

float Do_Allpass0(float inSample)
{
	float readback = apbuf0[ReverbData.ap0_p];
	readback += (-ReverbData.ap0_g) * inSample;
	float new = readback*ReverbData.ap0_g + inSample;
	apbuf0[ReverbData.ap0_p] = new;
	ReverbData.ap0_p++;
	if (ReverbData.ap0_p == ReverbData.ap0_lim)
		ReverbData.ap0_p=0;
	return readback;
}

float Do_Allpass1(float inSample)
{
	float readback = apbuf1[ReverbData.ap1_p];
	readback += (-ReverbData.ap1_g) * inSample;
	float new = readback*ReverbData.ap1_g + inSample;
	apbuf1[ReverbData.ap1_p] = new;
	ReverbData.ap1_p++;
	if (ReverbData.ap1_p == ReverbData.ap1_lim)
		ReverbData.ap1_p=0;
	return readback;
}

float Do_Allpass2(float inSample)
{
	float readback = apbuf2[ReverbData.ap2_p];
	readback += (-ReverbData.ap2_g) * inSample;
	float new = readback*ReverbData.ap2_g + inSample;
	apbuf2[ReverbData.ap2_p] = new;
	ReverbData.ap2_p++;
	if (ReverbData.ap2_p == ReverbData.ap2_lim)
		ReverbData.ap2_p=0;
	return readback;
}

float Reverb(float inSample)
{
	float newsample = (Do_Comb0(inSample) + Do_Comb1(inSample) + Do_Comb2(inSample) + Do_Comb3(inSample))/4.0f;
	newsample = Do_Allpass0(newsample);
	newsample = Do_Allpass1(newsample);
	newsample = Do_Allpass2(newsample);
	return newsample;
}

void Do_Reverb(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;
	if ( (Effect[REVERB_EFFECT_ID].effect_enabled & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = (int) ((1.0f-ReverbData.wet)*(float)inputData[i] + ReverbData.wet*Reverb((float)inputData[i]));
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Reverb_init(void)
{
	Effect[REVERB_EFFECT_ID].num_params = 0;
	sprintf(Effect[REVERB_EFFECT_ID].effect_name,"Reverb");
	Effect[REVERB_EFFECT_ID].do_effect =  Do_Echo;
	Effect[REVERB_EFFECT_ID].effect_enabled = 0;

	//define ReverbData.reverb_time delay 0.0 <-> 1.0 (max)
	bzero((uint8_t *)&ReverbData,sizeof(ReverbData));
	ReverbData.reverb_time = 1.0f;
	ReverbData.wet = 1.0f;
	ReverbData.cf0_lim = (int)(ReverbData.reverb_time*l_CB0);
	ReverbData.cf1_lim = (int)(ReverbData.reverb_time*l_CB1);
	ReverbData.cf2_lim = (int)(ReverbData.reverb_time*l_CB2);
	ReverbData.cf3_lim = (int)(ReverbData.reverb_time*l_CB3);
	ReverbData.ap0_lim = (int)(ReverbData.reverb_time*l_AP0);
	ReverbData.ap1_lim = (int)(ReverbData.reverb_time*l_AP1);
	ReverbData.ap2_lim = (int)(ReverbData.reverb_time*l_AP2);
	bzero(cfbuf0,l_CB0*sizeof(float));
	bzero(cfbuf1,l_CB1*sizeof(float));
	bzero(cfbuf2,l_CB2*sizeof(float));
	bzero(cfbuf3,l_CB3*sizeof(float));
	bzero(apbuf0,l_AP0*sizeof(float));
	bzero(apbuf1,l_AP1*sizeof(float));
	bzero(apbuf2,l_AP2*sizeof(float));
	ReverbData.cf0_g = 0.805f;
	ReverbData.cf1_g = 0.827f;
	ReverbData.cf2_g = 0.783f;
	ReverbData.cf3_g = 0.764f;
	ReverbData.ap0_g = 0.7f;
	ReverbData.ap1_g = 0.7f;
	ReverbData.ap2_g = 0.7f;
	//buffer-pointer are cleared by bzero, no need for these lines. Here to clarify only.
	/*
	ReverbData.cf0_p=0;
	ReverbData.cf1_p=0;
	ReverbData.cf2_p=0;
	ReverbData.cf3_p=0;
	ReverbData.ap0_p=0;
	ReverbData.ap1_p=0;
	ReverbData.ap2_p=0;
	*/
}

void Reverb_enable(void)
{
	Effect[REVERB_EFFECT_ID].effect_enabled |= EFFECT_ENABLED;
}

void Reverb_disable(void)
{
	Effect[REVERB_EFFECT_ID].effect_enabled &= ~EFFECT_ENABLED;
}

#endif // #ifdef SYNTH_ENGINE_ENABLE
