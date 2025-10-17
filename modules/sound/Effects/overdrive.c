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
 * overdrive.c
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */


#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"
#ifdef SOUND_ENABLED

#include "overdrive.h"

ITCM_AREA_CODE static float overdrive_process(OVERDRIVE_Effect_TypeDef* overdrive, float input)
{
	input *= overdrive->drive; // e.g., drive = 5.0
    return tanhf(input); // soft clip
}

ITCM_AREA_CODE void Effect_Overdrive(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
OVERDRIVE_Effect_TypeDef *overdrive = (OVERDRIVE_Effect_TypeDef *)effect->private_data;

	if ((( overdrive->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( overdrive == NULL ))
		return;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( overdrive->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			effect->out_buf[i + start_sample] = (q15_t ) overdrive_process(overdrive,(float )effect->in_buf[i]);
		else
			effect->out_buf[i + start_sample]  = effect->in_buf[i];
	}
}
#endif // #ifdef SOUND_ENABLED
