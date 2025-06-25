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
 *  Created on: Apr 24, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"
#ifdef SOUND_ENABLED

#include "passthrough.h"


ITCM_AREA_CODE void Effect_Passthrough(uint32_t *effect_s, uint32_t start_sample)
{
uint32_t	i;
Effect_TypeDef *effect = (Effect_TypeDef *)effect_s;
PASSTHROUGH_Effect_TypeDef *private = (PASSTHROUGH_Effect_TypeDef *)effect->private_data;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		effect->out_buf[i + start_sample]  = effect->in_buf[i]+effect->out_device;
	private->call_counter++;
}
#endif // #ifdef SOUND_ENABLED


