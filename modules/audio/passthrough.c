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
 * passthrough.c
 *
 *  Created on: Feb 26, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"				/* for audio parameters */
#include "../../kernel/kernel_opt.h"

#include "effects.h"
ITCM_AREA_CODE void Do_PassThrough(int16_t *inputData, int16_t *outputData)
{
uint32_t	i;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		outputData[i] = inputData[i];
}


void PassThrough_init(void)
{
	Effect[PASSTHROUGH_EFFECT_ID].num_params = 0;
	sprintf(Effect[PASSTHROUGH_EFFECT_ID].effect_name,"PassThrough");
	Effect[PASSTHROUGH_EFFECT_ID].apply_effect =  Do_PassThrough;
	Effect[PASSTHROUGH_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}

void PassThrough_enable(void)
{
	Effect[PASSTHROUGH_EFFECT_ID].effect_status |= EFFECT_ENABLED;
}

void PassThrough_disable(void)
{
	Effect[PASSTHROUGH_EFFECT_ID].effect_status &= ~EFFECT_ENABLED;
}
#endif //#ifdef SYNTH_ENGINE_ENABLE


