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
 * Project : bb1xx_743_00 
*/
/*
 * vca.c
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
ITCM_AREA_CODE void Do_Vca(int16_t *inputData, int16_t *outputData)
{
	if ( (Effect[VCA_EFFECT_ID].effect_status & EFFECT_ENABLED) == EFFECT_ENABLED )
			*outputData = (int16_t )((float )*inputData * Effect[VCA_EFFECT_ID].parameter[0]);
	else
			*outputData = *inputData;
}

void Vca_setMasterVolume(uint8_t Volume)
{
	Effect[VCA_EFFECT_ID].parameter[0] = (Volume <= 100) ? (float )Volume / 100.0F : 1.0F;
}

void Vca_init(uint8_t Volume)
{
	Effect[VCA_EFFECT_ID].parameter[0] = (float )Volume / 100.0F;
	Effect[VCA_EFFECT_ID].num_params = 1;
	sprintf(Effect[VCA_EFFECT_ID].effect_name,"Vca");
	sprintf(Effect[VCA_EFFECT_ID].effect_param[0],"Volume");
	Effect[VCA_EFFECT_ID].do_effect =  Do_Echo;
	Effect[VCA_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

void Vca_enable(void)
{
	Effect[VCA_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void Vca_disable(void)
{
	Effect[VCA_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}
#endif //#ifdef SYNTH_ENGINE_ENABLE
