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



D2_BUFFER	float cfbuf0[l_CB0], cfbuf1[l_CB1], cfbuf2[l_CB2], cfbuf3[l_CB3];

/*
//define wet 0.0 <-> 1.0
OSCILLATORS_RAM	float wet = 1.0f;
//define time delay 0.0 <-> 1.0 (max)
OSCILLATORS_RAM	float time = 1.0f;

//define pointer limits = delay time
OSCILLATORS_RAM	int cf0_lim, cf1_lim, cf2_lim, cf3_lim, ap0_lim, ap1_lim, ap2_lim;

//define buffer for comb- and allpassfilters
OSCILLATORS_RAM float apbuf0[l_AP0], apbuf1[l_AP1], apbuf2[l_AP2];
//feedback defines as of Schroeder
OSCILLATORS_RAM	float cf0_g = 0.805f, cf1_g=0.827f, cf2_g=0.783f, cf3_g=0.764f;
OSCILLATORS_RAM	float ap0_g = 0.7f, ap1_g = 0.7f, ap2_g = 0.7f;
//buffer-pointer
OSCILLATORS_RAM	int cf0_p=0, cf1_p=0, cf2_p=0, cf3_p=0, ap0_p=0, ap1_p=0, ap2_p=0;
*/

OSCILLATORS_RAM	ReverbTypeDef	ReverbData;

ITCM_AREA_CODE	float Do_Comb0(float inSample)
{
float readback,new;

	readback = cfbuf0[ReverbData.cf0_p];
	new = readback*ReverbData.cf0_g + inSample;
	cfbuf0[ReverbData.cf0_p] = new;
	ReverbData.cf0_p++;
	if (ReverbData.cf0_p==ReverbData.cf0_lim)
		ReverbData.cf0_p = 0;
	return readback;
}

ITCM_AREA_CODE	float Do_Comb1(float inSample)
{
float readback,new;

	readback = cfbuf1[ReverbData.cf1_p];
	new = readback*ReverbData.cf1_g + inSample;
	cfbuf1[ReverbData.cf1_p] = new;
	ReverbData.cf1_p++;
	if (ReverbData.cf1_p==ReverbData.cf1_lim)
		ReverbData.cf1_p = 0;
	return readback;
}

ITCM_AREA_CODE	float Do_Comb2(float inSample)
{
float readback,new;

	readback = cfbuf2[ReverbData.cf2_p];
	new = readback*ReverbData.cf2_g + inSample;
	cfbuf2[ReverbData.cf2_p] = new;
	ReverbData.cf2_p++;
	if (ReverbData.cf2_p==ReverbData.cf2_lim)
		ReverbData.cf2_p = 0;
	return readback;
}

ITCM_AREA_CODE	float Do_Comb3(float inSample)
{
float readback,new;

	readback = cfbuf3[ReverbData.cf3_p];
	new = readback*ReverbData.cf3_g + inSample;
	cfbuf3[ReverbData.cf3_p] = new;
	ReverbData.cf3_p++;
	if (ReverbData.cf3_p==ReverbData.cf3_lim)
		ReverbData.cf3_p = 0;
	return readback;
}

ITCM_AREA_CODE	float Do_Allpass0(float inSample)
{
float readback,new;

	readback = ReverbData.apbuf0[ReverbData.ap0_p];
	readback += (-ReverbData.ap0_g) * inSample;
	new = readback*ReverbData.ap0_g + inSample;
	ReverbData.apbuf0[ReverbData.ap0_p] = new;
	ReverbData.ap0_p++;
	if (ReverbData.ap0_p == ReverbData.ap0_lim)
		ReverbData.ap0_p=0;
	return readback;
}

ITCM_AREA_CODE	float Do_Allpass1(float inSample)
{
float readback,new;

	readback = ReverbData.apbuf1[ReverbData.ap1_p];
	readback += (-ReverbData.ap1_g) * inSample;
	new = readback*ReverbData.ap1_g + inSample;
	ReverbData.apbuf1[ReverbData.ap1_p] = new;
	ReverbData.ap1_p++;
	if (ReverbData.ap1_p == ReverbData.ap1_lim)
		ReverbData.ap1_p=0;
	return readback;
}

ITCM_AREA_CODE	float Do_Allpass2(float inSample)
{
float readback,new;

	readback = ReverbData.apbuf2[ReverbData.ap2_p];
	readback += (-ReverbData.ap2_g) * inSample;
	new = readback*ReverbData.ap2_g + inSample;
	ReverbData.apbuf2[ReverbData.ap2_p] = new;
	ReverbData.ap2_p++;
	if (ReverbData.ap2_p == ReverbData.ap2_lim)
		ReverbData.ap2_p=0;
	return readback;
}

ITCM_AREA_CODE	void Do_Reverb(int16_t* inputData, int16_t* outputData)
{
float newsample;
uint16_t	i;

	if ( (Effect[REVERB_EFFECT_ID].effect_enabled & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			newsample = (Do_Comb0((float )inputData[i]) + Do_Comb1((float )inputData[i]) + Do_Comb2((float )inputData[i]) + Do_Comb3((float )inputData[i]))/4.0f;
			newsample = Do_Allpass0(newsample);
			newsample = Do_Allpass1(newsample);
			newsample = Do_Allpass2(newsample);
			outputData[i] = (int16_t ) newsample;
		}
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
	Effect[REVERB_EFFECT_ID].do_effect =  Do_Reverb;
	Effect[REVERB_EFFECT_ID].effect_enabled = 0;

	ReverbData.wet = 1.0f;
	ReverbData.time = 1.0f;
	//feedback defines as of Schroeder
	ReverbData.cf0_g = 0.805f;
	ReverbData.cf1_g=0.827f;
	ReverbData.cf2_g=0.783f;
	ReverbData.cf3_g=0.764f;
	ReverbData.ap0_g = 0.7f;
	ReverbData.ap1_g = 0.7f;
	ReverbData.ap2_g = 0.7f;
	//buffer-pointer
	ReverbData.cf0_p = ReverbData.cf1_p = ReverbData.cf2_p = ReverbData.cf3_p = ReverbData.ap0_p = ReverbData.ap1_p = ReverbData.ap2_p = 0;
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
