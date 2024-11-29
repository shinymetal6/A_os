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
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
//#include "../../kernel/kernel_opt.h"

#include "../audio.h"
#include "../effects.h"
#include "vca.h"

ITCM_AREA_CODE void Do_Vca(int16_t *inputData, int16_t *outputData, uint8_t index)
{
uint32_t	i;
VCA_Effect_TypeDef	*VCA_Effect = (VCA_Effect_TypeDef *)Effects[index].private_data;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( VCA_Effect->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
				outputData[i] = (int16_t )((float )inputData[i] * VCA_Effect->volume);
		else
				outputData[i] = inputData[i];
	}
}

