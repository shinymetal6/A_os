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
 * audio.h
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#ifndef KERNEL_AUDIO_H_
#define KERNEL_AUDIO_H_

typedef struct
{
	int16_t		channel[2];
}WaveLR_t;

typedef struct _AudioFlagsTypeDef
{
	uint8_t 	audio_flags;
	uint8_t 	control_flags;
	float		master_volume;

}AudioFlagsTypeDef;

/* audio_flags values */
#define	AUDIO_HALFBUFIN_FLAG		0x01
#define	AUDIO_HALFBUFOUT_FLAG		0x02
#define	AUDIO_OUT_READY_FLAG		0x10
#define	AUDIO_IN_READY_FLAG			0x20
#define	AUDIO_GENERATE_FLAG			0x80

/* control_flags values */
/*
#define	CONTROL_OSC_FROM_CV			0x01
#define	CONTROL_OSC_FROM_MIDI		0x02
*/
#define	CONTROL_OSC_VCF_DLY			0x01
#define	CONTROL_FM					0x02
#define	CONTROL_VCA					0x04
#define	CONTROL_ROLLBACK2ADSR		0x08
#define	CONTROL_ADC_FLAG			0x10
#define	CONTROL_TICK_FLAG			0x20
#define	CONTROL_SYSTICK_FLAG		0x40
#define	CONTROL_MIDIRX_FLAG			0x80

#define	AUDIO_LEFT_CH		0
#define	AUDIO_RIGHT_CH		1

#define	NUMBER_OF_AUDIO_SAMPLES			AUDIO_BUF_SIZE
#define	HALF_NUMBER_OF_AUDIO_SAMPLES	(NUMBER_OF_AUDIO_SAMPLES/2)


#endif /* KERNEL_AUDIO_H_ */
