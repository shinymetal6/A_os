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
 * adsr.c
 *
 *  Created on: Feb 23, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effects.h"
#include "adsr.h"

AdsrTypeDef	AdsrData;

// this should be called every 1 mSec
ITCM_AREA_CODE void Do_Adsr_sm(void)
{
	if ( (BlockEffect[ADSR_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		switch (AdsrData.state )
		{
		case	IDLE:
			AdsrData.state = ATTACK;
			break;
		case	ATTACK:
			if ( (BlockEffect[ADSR_EFFECT_ID].effect_status & EFFECT_WAVE_ON) == EFFECT_WAVE_ON )
			{
				AdsrData.OutLlevel = AdsrData.AttackStartLevel;
				AdsrData.AttackStartLevel += AdsrData.AttackDelta;
				AdsrData.AttackRate--;
				if (AdsrData.AttackRate == 0 )
				{
					AdsrData.AttackStartLevel = AdsrData.AttackStartLevel_WORK;
					AdsrData.state = DECAY;
				}
			}
			break;
		case	DECAY:
			AdsrData.OutLlevel = AdsrData.DecayLevel;
			AdsrData.DecayLevel -= AdsrData.DecayDelta;
			AdsrData.DecayRate--;
			if (AdsrData.DecayRate <= 0 )
			{
				AdsrData.DecayLevel = AdsrData.DecayLevel_WORK;
				AdsrData.state = SUSTAIN;
			}
			break;
		case	SUSTAIN:
			AdsrData.OutLlevel =  AdsrData.SustainLevel;
			if ( (BlockEffect[ADSR_EFFECT_ID].effect_status & EFFECT_WAVE_ON) != EFFECT_WAVE_ON )
				AdsrData.state = RELEASE;
			break;
		case	RELEASE:
			AdsrData.OutLlevel = AdsrData.ReleaseLevel_WORK;
			AdsrData.ReleaseLevel -= AdsrData.ReleaseDelta;
			if (AdsrData.ReleaseLevel <= 0 )
			{
				AdsrData.ReleaseLevel = AdsrData.ReleaseLevel_WORK;
				AdsrData.state = SUSTAIN;
			}
			break;
		}
	}
}

void Adsr_set(uint32_t AttackRate,uint32_t AttackStartLevel,uint32_t DecayRate,uint32_t SustainLevel,uint32_t ReleaseRate)
{
	BlockEffect[ADSR_EFFECT_ID].parameter[0] = (float )(AttackRate * SAMPLE_FREQUENCY);
	BlockEffect[ADSR_EFFECT_ID].parameter[1] = (float )AttackStartLevel;
	BlockEffect[ADSR_EFFECT_ID].parameter[2] = (float )(DecayRate * SAMPLE_FREQUENCY);
	BlockEffect[ADSR_EFFECT_ID].parameter[3] = (float )SustainLevel;
	BlockEffect[ADSR_EFFECT_ID].parameter[4] = (float )(ReleaseRate * SAMPLE_FREQUENCY);

	AdsrData.AttackRate = AttackRate;
	AdsrData.AttackStartLevel = AdsrData.AttackStartLevel_WORK = (float )AttackStartLevel;
	AdsrData.DecayRate = DecayRate;
	AdsrData.DecayLevel_WORK = (float )SustainLevel;
	AdsrData.SustainLevel = (float )SustainLevel;
	AdsrData.ReleaseRate = ReleaseRate;

	AdsrData.AttackDelta = (100.0f - ((float )AttackStartLevel) / (float )AttackRate);
	AdsrData.DecayDelta =  (100.0f - ((float )SustainLevel) / (float )AttackRate);
	AdsrData.ReleaseDelta =  (float )SustainLevel / (float )AttackRate;
}

ITCM_AREA_CODE void Do_Adsr(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if ( (BlockEffect[ADSR_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
			outputData[i] = (int16_t )((float )inputData[i] * AdsrData.OutLlevel);
		else
			outputData[i] = inputData[i];
	}
}

void Adsr_init(uint32_t AttackRate,uint32_t AttackStartLevel,uint32_t DecayRate,uint32_t SustainLevel,uint32_t ReleaseRate)
{
	BlockEffect[ADSR_EFFECT_ID].parameter[0] = (float )(AttackRate * SAMPLE_FREQUENCY);
	BlockEffect[ADSR_EFFECT_ID].parameter[1] = (float )AttackStartLevel;
	BlockEffect[ADSR_EFFECT_ID].parameter[2] = (float )(DecayRate * SAMPLE_FREQUENCY);
	BlockEffect[ADSR_EFFECT_ID].parameter[3] = (float )SustainLevel;
	BlockEffect[ADSR_EFFECT_ID].parameter[4] = (float )(ReleaseRate * SAMPLE_FREQUENCY);
	BlockEffect[ADSR_EFFECT_ID].num_params = 5;

	sprintf(BlockEffect[ADSR_EFFECT_ID].effect_name,"Adsr");
	sprintf(BlockEffect[ADSR_EFFECT_ID].effect_param[0],"AttackRate[mSec]");
	sprintf(BlockEffect[ADSR_EFFECT_ID].effect_param[1],"AttackStartLevel[0..1]");
	sprintf(BlockEffect[ADSR_EFFECT_ID].effect_param[2],"DecayRate[mSec]");
	sprintf(BlockEffect[ADSR_EFFECT_ID].effect_param[3],"SustainLevel[0..1]");
	sprintf(BlockEffect[ADSR_EFFECT_ID].effect_param[4],"ReleaseRate[mSec]");
	BlockEffect[ADSR_EFFECT_ID].apply_effect =  Do_Adsr;
	BlockEffect[ADSR_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;

	AdsrData.AttackRate = AttackRate;
	AdsrData.AttackStartLevel = AdsrData.AttackStartLevel_WORK = (float )AttackStartLevel;
	AdsrData.DecayRate = DecayRate;
	AdsrData.SustainLevel = AdsrData.DecayLevel_WORK = (float )SustainLevel;
	AdsrData.ReleaseRate = ReleaseRate;

	AdsrData.AttackDelta = (100.0f - ((float )AttackStartLevel) / (float )AttackRate);
	AdsrData.DecayDelta =  (100.0f - ((float )SustainLevel) / (float )AttackRate);
	AdsrData.ReleaseDelta =  (float )SustainLevel / (float )AttackRate;

	AdsrData.state = IDLE;
	AdsrData.OutLlevel = 1.0f;
}


void Adsr_enable(void)
{
	BlockEffect[ADSR_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Adsr_disable(void)
{
	BlockEffect[ADSR_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif
