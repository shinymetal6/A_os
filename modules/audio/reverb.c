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
#include "reverb.h"

//Schroeder delays from 25k->96k interpolated
//*2 delay extension -> not more possible without external ram
#define l_CB0 3460*2
#define l_CB1 2988*2
#define l_CB2 3882*2
#define l_CB3 4312*2
#define l_AP0 480*2
#define l_AP1 161*2
#define l_AP2 46*2

D2_BUFFER	float cfbuf0[l_CB0], cfbuf1[l_CB1], cfbuf2[l_CB2], cfbuf3[l_CB3];

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

ITCM_AREA_CODE	float Do_Comb0(float inSample)
{
float readback,new;

	readback = cfbuf0[cf0_p];
	new = readback*cf0_g + inSample;
	cfbuf0[cf0_p] = new;
	cf0_p++;
	if (cf0_p==cf0_lim) cf0_p = 0;
		return readback;
}

ITCM_AREA_CODE	float Do_Comb1(float inSample)
{
float readback,new;

	readback = cfbuf1[cf1_p];
	new = readback*cf1_g + inSample;
	cfbuf1[cf1_p] = new;
	cf1_p++;
	if (cf1_p==cf1_lim) cf1_p = 0;
		return readback;
}

ITCM_AREA_CODE	float Do_Comb2(float inSample)
{
float readback,new;

	readback = cfbuf2[cf2_p];
	new = readback*cf2_g + inSample;
	cfbuf2[cf2_p] = new;
	cf2_p++;
	if (cf2_p==cf2_lim) cf2_p = 0;
		return readback;
}

ITCM_AREA_CODE	float Do_Comb3(float inSample)
{
float readback,new;

	readback = cfbuf3[cf3_p];
	new = readback*cf3_g + inSample;
	cfbuf3[cf3_p] = new;
	cf3_p++;
	if (cf3_p==cf3_lim) cf3_p = 0;
		return readback;
}

ITCM_AREA_CODE	float Do_Allpass0(float inSample)
{
float readback,new;

	readback = apbuf0[ap0_p];
	readback += (-ap0_g) * inSample;
	new = readback*ap0_g + inSample;
	apbuf0[ap0_p] = new;
	ap0_p++;
	if (ap0_p == ap0_lim) ap0_p=0;
		return readback;
}

ITCM_AREA_CODE	float Do_Allpass1(float inSample)
{
float readback,new;

	readback = apbuf1[ap1_p];
	readback += (-ap1_g) * inSample;
	new = readback*ap1_g + inSample;
	apbuf1[ap1_p] = new;
	ap1_p++;
	if (ap1_p == ap1_lim) ap1_p=0;
		return readback;
}

ITCM_AREA_CODE	float Do_Allpass2(float inSample)
{
float readback,new;

	readback = apbuf2[ap2_p];
	readback += (-ap2_g) * inSample;
	new = readback*ap2_g + inSample;
	apbuf2[ap2_p] = new;
	ap2_p++;
	if (ap2_p == ap2_lim) ap2_p=0;
		return readback;
}

ITCM_AREA_CODE	float Do_Reverb(float inSample)
{
float newsample;

	newsample = (Do_Comb0(inSample) + Do_Comb1(inSample) + Do_Comb2(inSample) + Do_Comb3(inSample))/4.0f;
	newsample = Do_Allpass0(newsample);
	newsample = Do_Allpass1(newsample);
	newsample = Do_Allpass2(newsample);
	return newsample;
}

#endif // #ifdef SYNTH_ENGINE_ENABLE
