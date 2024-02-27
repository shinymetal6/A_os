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
 * vca_s.c
 *
 *  Created on: Feb 27, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effects.h"
ITCM_AREA_CODE void Do_Vca_s(int16_t *inputData, int16_t *outputData)
{
	if ( (Effect[VCA_S_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
			*outputData = (int16_t )((float )*inputData * Effect[VCA_S_EFFECT_ID].parameter[0]);
	else
			*outputData = *inputData;
}

void Vca_s_setMasterVolume(uint8_t Volume)
{
	Effect[VCA_S_EFFECT_ID].parameter[0] = (Volume <= 100) ? (float )Volume / 100.0F : 1.0F;
}

void Vca_s_init(uint8_t Volume)
{
	Effect[VCA_S_EFFECT_ID].parameter[0] = (float )Volume / 100.0F;
	Effect[VCA_S_EFFECT_ID].num_params = 1;
	sprintf(Effect[VCA_S_EFFECT_ID].effect_name,"Vca");
	sprintf(Effect[VCA_S_EFFECT_ID].effect_param[0],"Volume");
	Effect[VCA_S_EFFECT_ID].apply_effect =  Do_Vca_s;
	Effect[VCA_S_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

void Vca_s_enable(void)
{
	Effect[VCA_S_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Vca_s_disable(void)
{
	Effect[VCA_S_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}
#endif //#ifdef SYNTH_ENGINE_ENABLE


