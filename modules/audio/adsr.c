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

ITCM_AREA_CODE void Do_Adsr(int16_t* inputData, int16_t* outputData)
{
uint16_t	i;
float		out_level = 0.0f;
	if ( (Effect[ADSR_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			switch (AdsrData.state )
			{
			case	ATTACK:
				if ( (Effect[ADSR_EFFECT_ID].effect_status & EFFECT_WAVE_ON) == EFFECT_WAVE_ON )
				{
					out_level = AdsrData.AttackStartLevel;
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
				out_level = AdsrData.DecayLevel;
				AdsrData.DecayLevel -= AdsrData.DecayDelta;
				AdsrData.DecayRate--;
				if (AdsrData.DecayRate <= 0 )
				{
					AdsrData.DecayLevel = AdsrData.DecayLevel_WORK;
					AdsrData.state = SUSTAIN;
				}
				break;
			case	SUSTAIN:
				out_level =  AdsrData.SustainLevel;
				if ( (Effect[ADSR_EFFECT_ID].effect_status & EFFECT_WAVE_ON) != EFFECT_WAVE_ON )
					AdsrData.state = RELEASE;
				break;
			case	RELEASE:
				out_level = AdsrData.ReleaseLevel_WORK;
				AdsrData.ReleaseLevel -= AdsrData.ReleaseDelta;
				if (AdsrData.ReleaseLevel <= 0 )
				{
					AdsrData.ReleaseLevel = AdsrData.ReleaseLevel_WORK;
					AdsrData.state = SUSTAIN;
				}
				break;
			}
		}
		outputData[i] = (int16_t )((float )inputData[i] * out_level);
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}

}

void Adsr_init(uint32_t AttackRate,uint32_t AttackStartLevel,uint32_t DecayRate,uint32_t SustainLevel,uint32_t ReleaseRate)
{
	Effect[ADSR_EFFECT_ID].parameter[0] = (float )(AttackRate * SAMPLE_FREQUENCY);
	Effect[ADSR_EFFECT_ID].parameter[1] = (float )AttackStartLevel;
	Effect[ADSR_EFFECT_ID].parameter[2] = (float )(DecayRate * SAMPLE_FREQUENCY);
	Effect[ADSR_EFFECT_ID].parameter[3] = (float )SustainLevel;
	Effect[ADSR_EFFECT_ID].parameter[4] = (float )(ReleaseRate * SAMPLE_FREQUENCY);
	Effect[ADSR_EFFECT_ID].num_params = 5;

	sprintf(Effect[ADSR_EFFECT_ID].effect_name,"Adsr");
	sprintf(Effect[ADSR_EFFECT_ID].effect_param[0],"AttackRate");
	sprintf(Effect[ADSR_EFFECT_ID].effect_param[1],"AttackStartLevel");
	sprintf(Effect[ADSR_EFFECT_ID].effect_param[2],"DecayRate");
	sprintf(Effect[ADSR_EFFECT_ID].effect_param[3],"SustainLevel");
	sprintf(Effect[ADSR_EFFECT_ID].effect_param[4],"ReleaseRate");
	Effect[ADSR_EFFECT_ID].apply_effect =  Do_Adsr;
	Effect[ADSR_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;

	AdsrData.AttackRate = AttackRate;
	AdsrData.AttackStartLevel = AdsrData.AttackStartLevel_WORK = (float )AttackStartLevel;
	AdsrData.DecayRate = DecayRate;
	AdsrData.DecayLevel_WORK = (float )SustainLevel;
	AdsrData.SustainLevel = (float )SustainLevel;
	AdsrData.ReleaseRate = ReleaseRate;

	AdsrData.AttackDelta = (100.0f - ((float )AttackStartLevel) / (float )AttackRate);
	AdsrData.DecayDelta =  (100.0f - ((float )SustainLevel) / (float )AttackRate);
	AdsrData.ReleaseDelta =  (float )SustainLevel / (float )AttackRate;

	AdsrData.state = ATTACK;
}


void Adsr_enable(void)
{
	Effect[ADSR_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Adsr_disable(void)
{
	Effect[ADSR_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

#endif
