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
 * phaser.c
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

extern	float phaser_sinetable[PHASER_SINETABLE_SIZE+1];

OSCILLATORS_RAM	PhaserTypeDef	PhaserData;

ITCM_AREA_CODE void Phaser_Rate_set(uint8_t val)
{
	BlockEffect[PHASER_EFFECT_ID].parameter[2] = (MAX_RATE - MIN_RATE) / MIDI_MAX * val + MIN_RATE;
	PhaserData.lfoInc = 2 * PI * BlockEffect[PHASER_EFFECT_ID].parameter[2] / SAMPLE_FREQUENCY;
}

ITCM_AREA_CODE void Phaser_Feedback_set(uint8_t val)
{
	BlockEffect[PHASER_EFFECT_ID].parameter[3] = 0.999f * (float )val / MIDI_MAX;
}

ITCM_AREA_CODE void Phaser_Wet_set(uint8_t val)
{
	PhaserData.wet = (float )val / MIDI_MAX;
}

ITCM_AREA_CODE void PhaserRate(float rate)
{
	BlockEffect[PHASER_EFFECT_ID].parameter[2] = rate;
	PhaserData.lfoInc = 2 * PI * BlockEffect[PHASER_EFFECT_ID].parameter[2] / SAMPLE_FREQUENCY;
}

ITCM_AREA_CODE void PhaserFeedback(float fdb)
{
	BlockEffect[PHASER_EFFECT_ID].parameter[3] = fdb;
}

ITCM_AREA_CODE static float allpass(float yin, int ind)
{
	float yout;

	yout = - yin * PhaserData.a1 + PhaserData.old[ind];
	PhaserData.old[ind] = yout * PhaserData.a1 + yin;
	return yout;
}


ITCM_AREA_CODE void Do_Phaser(int16_t* inputData, int16_t* outputData)
{
float 		d,xin,yout;
int16_t		i;

	if ( (BlockEffect[PHASER_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			//calculate and update phaser sweep lfo...
			xin = (float )inputData[i];
			d  = PhaserData.dmin + (PhaserData.dmax - PhaserData.dmin) * ((phaser_sinetable[lrintf(ALPHA * PhaserData.lfoPhase)] + 1.f)*0.5f);

			PhaserData.lfoPhase += PhaserData.lfoInc;
			if( PhaserData.lfoPhase >= 2*PI )
				PhaserData.lfoPhase -= 2*PI;

			//update filter coeffs
			PhaserData.a1 = (1.f - d) / (1.f + d);

			//calculate output

			yout = allpass(xin + PhaserData.zm1 * BlockEffect[PHASER_EFFECT_ID].parameter[3], 0);

			for(i = 1; i < PH_STAGES; i++)
			{
				yout = allpass(yout, i);
			}
			PhaserData.zm1 = yout;

			yout = (1 - PhaserData.wet) * xin + PhaserData.wet * yout;

			outputData[i] = (int16_t )yout*0.8f;
			//return yout*0.8f;
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}

void Phaser_init(uint32_t Fmin,uint32_t Fmax,uint32_t SwRate,float Feedback)
{
	BlockEffect[PHASER_EFFECT_ID].parameter[0] = (float )Fmin;		//200.f;
	BlockEffect[PHASER_EFFECT_ID].parameter[1] = (float )Fmax;		//1700.f;
	BlockEffect[PHASER_EFFECT_ID].parameter[2] = (float )SwRate;		// 0.1f;
	BlockEffect[PHASER_EFFECT_ID].parameter[3] = Feedback;			// 0.7f;
	BlockEffect[PHASER_EFFECT_ID].num_params = 4;
	sprintf(BlockEffect[PHASER_EFFECT_ID].effect_name,"Phaser");
	sprintf(BlockEffect[PHASER_EFFECT_ID].effect_param[0],"Frequency Min");
	sprintf(BlockEffect[PHASER_EFFECT_ID].effect_param[1],"Frequency Max");
	sprintf(BlockEffect[PHASER_EFFECT_ID].effect_param[2],"Sweep Rate");
	sprintf(BlockEffect[PHASER_EFFECT_ID].effect_param[3],"Feedback");
	BlockEffect[PHASER_EFFECT_ID].apply_effect =  Do_Phaser;
	BlockEffect[PHASER_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;

	PhaserData.wet = 0.3f;
	PhaserData.dmin = 2 * BlockEffect[PHASER_EFFECT_ID].parameter[0] / SAMPLE_FREQUENCY;
	PhaserData.dmax = 2 * BlockEffect[PHASER_EFFECT_ID].parameter[1] / SAMPLE_FREQUENCY;
	PhaserData.lfoInc = 2 * PI * BlockEffect[PHASER_EFFECT_ID].parameter[2] / SAMPLE_FREQUENCY;
}


void Phaser_enable(void)
{
	BlockEffect[PHASER_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Phaser_disable(void)
{
	BlockEffect[PHASER_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif
