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
 * pitch_shift.c
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "pitch_shift.h"

int WtrP;
float Rd_P;
float Shift;
float CrossFade;
float a0, a1, a2, b1, b2, hp_in_z1, hp_in_z2, hp_out_z1, hp_out_z2;
#define BufSize 1000
#define Overlap 100

int Buf[BufSize];

void PitchShift_init(void)
{
	WtrP = 0;
	Rd_P = 0.0f;
	Shift = 1.4f;
	CrossFade = 1.0f;
}

int PitchShift_HighPass (int inSample)
{
	//300Hz high-pass, 96k
	a0 = 0.9862117951198142f;
	a1 = -1.9724235902396283f;
	a2 = 0.9862117951198142f;
	b1 = -1.972233470205696f;
	b2 = 0.9726137102735608f;

	float inSampleF = (float)inSample;
	float outSampleF =	a0 * inSampleF
						+ a1 * hp_in_z1
						+ a2 * hp_in_z2
						- b1 * hp_out_z1
						- b2 * hp_out_z2;
	hp_in_z2 = hp_in_z1;
	hp_in_z1 = inSampleF;
	hp_out_z2 = hp_out_z1;
	hp_out_z1 = outSampleF;
	return (int) outSampleF;
}

int Do_PitchShift(int lSample, int rSample)
{
	int sum = lSample + rSample;
	//sum up and do high-pass
	sum=PitchShift_HighPass(sum);

	//write to ringbuffer
	Buf[WtrP] = sum;

	//read fractional readpointer and generate 0° and 180° read-pointer in integer
	int RdPtr_Int = roundf(Rd_P);
	int RdPtr_Int2 = 0;
	if (RdPtr_Int >= BufSize/2)
		RdPtr_Int2 = RdPtr_Int - (BufSize/2);
	else
		RdPtr_Int2 = RdPtr_Int + (BufSize/2);

	//read the two samples...
	float Rd0 = (float) Buf[RdPtr_Int];
	float Rd1 = (float) Buf[RdPtr_Int2];

	//Check if first readpointer starts overlap with write pointer?
	// if yes -> do cross-fade to second read-pointer
	if (Overlap >= (WtrP-RdPtr_Int) && (WtrP-RdPtr_Int) >= 0 && Shift!=1.0f)
	{
		int rel = WtrP-RdPtr_Int;
		CrossFade = ((float)rel)/(float)Overlap;
	}
	else if (WtrP-RdPtr_Int == 0)
		CrossFade = 0.0f;

	//Check if second readpointer starts overlap with write pointer?
	// if yes -> do cross-fade to first read-pointer
	if (Overlap >= (WtrP-RdPtr_Int2) && (WtrP-RdPtr_Int2) >= 0 && Shift!=1.0f)
	{
		int rel = WtrP-RdPtr_Int2;
		CrossFade = 1.0f - ((float)rel)/(float)Overlap;
	}
	else if (WtrP-RdPtr_Int2 == 0)
		CrossFade = 1.0f;


	//do cross-fading and sum up
	sum = (Rd0*CrossFade + Rd1*(1.0f-CrossFade));

	//increment fractional read-pointer and write-pointer
	Rd_P += Shift;
	WtrP++;
	if (WtrP == BufSize)
		WtrP = 0;
	if (roundf(Rd_P) >= BufSize)
		Rd_P = 0.0f;
	return sum;
}

#endif

