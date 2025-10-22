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
 * i2s_in.c
 *
 *  Created on: Oct 22, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_I2S_ENABLED
#include "../sound_engine.h"

extern	Synth_TypeDef *AudioSource[2];
ITCM_AREA_CODE uint8_t I2SIn_Start(AudioSource_TypeDef *i2s_in)
{
	i2s_in->status = SOURCE_ENABLED;
    return 0;
}

ITCM_AREA_CODE uint8_t I2SIn_Stop(AudioSource_TypeDef *i2s_in)
{
	i2s_in->status &= ~SOURCE_DISABLED;
    return 0;
}

ITCM_AREA_CODE uint8_t I2SIn_Register(uint8_t channel,AudioSource_TypeDef *i2s_in)
{
	if (( i2s_in->codec_buf == NULL ) || ( i2s_in->synth_out_buf == NULL ))
		return 1;
	AudioSource[channel] = i2s_in;
	i2s_in->OutFunc = i2sin_to_i2sout;
	i2s_in->source_type = SYNTH_IS_I2S_IN;
	return 0;

}

#endif // #ifdef SOUND_ENGINE_I2S_ENABLED



