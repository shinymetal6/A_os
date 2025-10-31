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
 * schroeder_reverb.c
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "effects.h"
#include "schroeder_reverb.h"
#include <math.h>

ITCM_AREA_CODE void schroeder_reverb_init(uint32_t *effect_s)
{
SCHROEDER_REVERB_Effect_TypeDef *schroeder_reverb = (SCHROEDER_REVERB_Effect_TypeDef *)effect_s;
	// Delay lines
	bzero(schroeder_reverb->ap1_buffer,AP1_DELAY);
	bzero(schroeder_reverb->ap2_buffer,AP2_DELAY);
	bzero(schroeder_reverb->ap3_buffer,AP3_DELAY);
	bzero(schroeder_reverb->ap4_buffer,AP4_DELAY);
	// Read/write pointers
	schroeder_reverb->ap1_r = schroeder_reverb->ap1_w = 0;
	schroeder_reverb->ap2_r = schroeder_reverb->ap2_w = 0;
	schroeder_reverb->ap3_r = schroeder_reverb->ap3_w = 0;
	schroeder_reverb->ap4_r = schroeder_reverb->ap4_w = 0;
	//Reflection coefficients (|k| < 1.0)
	schroeder_reverb->K1  = 0.75f;
	schroeder_reverb->K2  = 0.70f;
	schroeder_reverb->K3  = 0.65f;
	schroeder_reverb->K4  = 0.60f;
}

ITCM_AREA_CODE static inline float allpass_stage(float input, float k,float *buffer, uint16_t size,uint16_t *read_ptr, uint16_t *write_ptr)
{
	float delay_out = buffer[*read_ptr];
	float output = -k * input + delay_out;
	buffer[*write_ptr] = input + k * output;

	// Circular increment
	*read_ptr = (*read_ptr + 1) % size;
	*write_ptr = (*write_ptr + 1) % size;

	return output;
}

ITCM_AREA_CODE static q15_t schroeder_reverb_effect(SCHROEDER_REVERB_Effect_TypeDef *schroeder_reverb,float input)
{
float x = input;

	// Cascade 4 all-pass filters
	x = allpass_stage(x, schroeder_reverb->K1, schroeder_reverb->ap1_buffer, AP1_DELAY, &schroeder_reverb->ap1_r, &schroeder_reverb->ap1_w);
	x = allpass_stage(x, schroeder_reverb->K2, schroeder_reverb->ap2_buffer, AP2_DELAY, &schroeder_reverb->ap2_r, &schroeder_reverb->ap2_w);
	x = allpass_stage(x, schroeder_reverb->K3, schroeder_reverb->ap3_buffer, AP3_DELAY, &schroeder_reverb->ap3_r, &schroeder_reverb->ap3_w);
	x = allpass_stage(x, schroeder_reverb->K4, schroeder_reverb->ap4_buffer, AP4_DELAY, &schroeder_reverb->ap4_r, &schroeder_reverb->ap4_w);

	return x;
}

ITCM_AREA_CODE void Effect_Schroeder_Reverb(uint32_t *effect_s)
{
uint32_t	i;
SCHROEDER_REVERB_Effect_TypeDef *schroeder_reverb = (SCHROEDER_REVERB_Effect_TypeDef *)effect_s;

	if ((( schroeder_reverb->status & SOUND_EFFECT_INITIALIZED) != SOUND_EFFECT_INITIALIZED) || ( schroeder_reverb == NULL ))
		return;
	schroeder_reverb->time_start = DWT->CYCCNT;
	for ( i=0;i<schroeder_reverb->block_size;i++)
	{
		if (( schroeder_reverb->flags & SOUND_EFFECT_ENABLED) == SOUND_EFFECT_ENABLED)
			schroeder_reverb->out_buf[i] = (q15_t ) schroeder_reverb_effect(schroeder_reverb,__Q15_2_FLOAT(schroeder_reverb->in_buf[i]));
		else
			schroeder_reverb->out_buf[i]  = schroeder_reverb->in_buf[i];
	}
	if (( schroeder_reverb->flags & SCHROEDER_REVERB_UPDATE_PARAMS) == SCHROEDER_REVERB_UPDATE_PARAMS)
	{
		schroeder_reverb_init(effect_s);
		schroeder_reverb->flags &= ~SCHROEDER_REVERB_UPDATE_PARAMS;
	}
	schroeder_reverb->effect_time = (DWT->CYCCNT - schroeder_reverb->time_start) / (HSI_CLOCK / 1000000);
}
#endif // #ifdef SOUND_ENABLED
