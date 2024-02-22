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
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */
#include "../../kernel/kernel_opt.h"

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"
#include "oscillators.h"
#include "oscillator_core.h"

extern	AudioFlagsTypeDef	AudioFlags;

float	vco_am = 0.5F;
/*
ITCM_AREA_CODE void VcaOLD( uint16_t *buffer_out,uint16_t *buffer_in)
{
uint32_t	i;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		if (( AudioFlags.control_flags & CONTROL_VCA) == CONTROL_VCA)
			*buffer_out++ = (uint16_t )((float )*buffer_in++ * vco_am );
		else
			*buffer_out++ = *buffer_in++;
	}
}
*/
int16_t	left_buf_in[HALF_NUMBER_OF_AUDIO_SAMPLES],right_buf_in[HALF_NUMBER_OF_AUDIO_SAMPLES];
int16_t	left_buf_out[HALF_NUMBER_OF_AUDIO_SAMPLES],right_buf_out[HALF_NUMBER_OF_AUDIO_SAMPLES];

//#define	VCA_ONLY
#define	REVERB

#ifndef REVERB
#define	IIR
#endif
ITCM_AREA_CODE void Vca( WaveLR_t *buffer_out,WaveLR_t *buffer_in,uint16_t	start)
{
uint32_t	i;
#ifdef VCA_ONLY
/*
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		buffer_out[i+start].channel[AUDIO_LEFT_CH]  = (int16_t )((float )buffer_in[i+start].channel[AUDIO_LEFT_CH]  * AudioFlags.master_volume);
		buffer_out[i+start].channel[AUDIO_RIGHT_CH] = (int16_t )((float )buffer_in[i+start].channel[AUDIO_RIGHT_CH] * AudioFlags.master_volume);
	}
	*/
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		buffer_out[i+start].channel[AUDIO_LEFT_CH]  = (int16_t )((float )buffer_in[i+start].channel[AUDIO_LEFT_CH] );
		buffer_out[i+start].channel[AUDIO_RIGHT_CH] = (int16_t )((float )buffer_in[i+start].channel[AUDIO_RIGHT_CH]);
	}
#endif
#ifdef IIR
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		left_buf_in[i] = (int16_t )((float )buffer_in[i+start].channel[AUDIO_LEFT_CH]);
		right_buf_in[i] = (int16_t)((float )buffer_in[i+start].channel[AUDIO_RIGHT_CH]);
	}
	iir(left_buf_out , left_buf_in);
	iir(right_buf_out , right_buf_in);

	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		buffer_out[i+start].channel[AUDIO_LEFT_CH]  = (int16_t )((float )left_buf_out[i] * AudioFlags.master_volume);
		buffer_out[i+start].channel[AUDIO_RIGHT_CH] = (int16_t )((float )right_buf_out[i]* AudioFlags.master_volume);
	}
#endif
#ifdef REVERB
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		buffer_out[i+start].channel[AUDIO_LEFT_CH]   = (int16_t )Do_Reverb((float )buffer_in[i+start].channel[AUDIO_LEFT_CH]);
		buffer_out[i+start].channel[AUDIO_RIGHT_CH]  = (int16_t )Do_Reverb((float )buffer_in[i+start].channel[AUDIO_RIGHT_CH]);
	}
#endif
}

#endif //#ifdef SYNTH_ENGINE_ENABLE
