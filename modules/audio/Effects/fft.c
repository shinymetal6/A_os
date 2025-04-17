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
 * fft.c
 *
 *  Created on: Apr 16, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"
#include "fft.h"
#include "arm_math.h"
arm_biquad_casd_df1_inst_f32 fft_iirsettings;
arm_rfft_fast_instance_f32 instance;

arm_status status;

#define	FORWARD_FFT		0
#define	INVERSE_FFT		1

float	float_window_coeffs[HALF_NUMBER_OF_AUDIO_SAMPLES];

float	float_inbuf[HALF_NUMBER_OF_AUDIO_SAMPLES];
float	float_fft_outbuf[HALF_NUMBER_OF_AUDIO_SAMPLES];
float	float_fft_outbuf_reverse[HALF_NUMBER_OF_AUDIO_SAMPLES];
float	float_fftmag_outbuf[HALF_NUMBER_OF_AUDIO_SAMPLES/2];

void apply_window (int16_t fft_window,int16_t *inputData)
{
	uint32_t	i;
	switch(fft_window)
	{
	case FFT_WINDOW_NONE :
		for ( i = 0; i < HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
		{
			float_window_coeffs[i] =  1.0F;
			float_inbuf[i] = (float )inputData[i];
		}
		break;
	case FFT_WINDOW_BLACKMAN :
		for ( i = 0; i < HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
		{
			float_window_coeffs[i] =  ( 0.42f - 0.5f * cos (2.0f*PI*(float)i/(float)(HALF_NUMBER_OF_AUDIO_SAMPLES-1)) + 0.08f * cos (4.0f*PI*(float)i/(float)(HALF_NUMBER_OF_AUDIO_SAMPLES-1)) );
			float_inbuf[i] = (float )inputData[i] * float_window_coeffs[i];
		}
		break;
	case FFT_WINDOW_HAMMING :
	    for (i = 0; i < HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
	    {
	    	float_window_coeffs[i] = 0.54f - (0.46f * cos(((2.0 * PI) / HALF_NUMBER_OF_AUDIO_SAMPLES) * (i+ 0.5f)));
	    	float_inbuf[i] = (float )inputData[i] * float_window_coeffs[i];
	    }
		break;
	case FFT_WINDOW_HANNING :
		for ( i = 0; i < HALF_NUMBER_OF_AUDIO_SAMPLES; i++)
		{
			float_window_coeffs[i] = 0.5f * (1 - cos(2*PI*i/(HALF_NUMBER_OF_AUDIO_SAMPLES-1)));
			float_inbuf[i] = (float )inputData[i] * float_window_coeffs[i];
		}
		break;
	}
}

void Fft_init(FFT_Effect_TypeDef *fft)
{

}

arm_rfft_fast_instance_f32 fft_instance;
ITCM_AREA_CODE void Do_Fft(int16_t *inputData, int16_t *outputData, uint8_t index)
{
FFT_Effect_TypeDef *fft	= (FFT_Effect_TypeDef *)Effects[index].private_data;
uint32_t	i;

	if (( fft->flags & EFFECT_INITIALIZED) == 0)
	{
		arm_rfft_fast_init_f32(&fft_instance, HALF_NUMBER_OF_AUDIO_SAMPLES);
		fft->flags |= EFFECT_INITIALIZED;
	}
	if (( fft->flags & EFFECT_ENABLED ) == EFFECT_ENABLED )
	{
		arm_rfft_fast_f32(&fft_instance, float_inbuf, float_fft_outbuf, FORWARD_FFT);
		arm_rfft_fast_f32(&fft_instance, float_fft_outbuf, float_fft_outbuf_reverse, INVERSE_FFT);
		arm_cmplx_mag_f32(float_fft_outbuf,float_fftmag_outbuf, HALF_NUMBER_OF_AUDIO_SAMPLES/2);
		for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		{
			outputData[i] = (int16_t)float_fft_outbuf_reverse[i];
		}
	}
	else
	{
		for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			outputData[i] = inputData[i];
	}
}
