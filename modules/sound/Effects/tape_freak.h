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
 * tape_freak.h
 *
 *  Created on: May 17, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_TAPE_FREAK_H_
#define MODULES_SOUND_EFFECTS_TAPE_FREAK_H_
#ifdef SOUND_ENABLED

#define TAPE_FREAK_BUFFER_SIZE		SOUND_BLOCK_SIZE  	// 128 samples
#define TAPE_FREAK_SAMPLE_RATE		DEFAULT_SAMPLE_FREQUENCY

// Tone control ranges
#define TAPE_FREAK_MAX_GAIN 4.0f
#define TAPE_FREAK_MAX_BASS_GAIN 2.0f
#define TAPE_FREAK_MAX_TREBLE_GAIN 2.0f

typedef struct
{
	uint8_t		flags;
	float32_t	tone_gain_bass;
	float32_t	tone_gain_treble;
	float32_t	preamp_gain;
	/* internals */
	float32_t	lpfState[4], bassState[4], trebleState[4];
	float32_t	biquadCoeffsLP[5],biquadCoeffsBass[5],biquadCoeffsTreble[5];
	arm_biquad_cascade_df2T_instance_f32 LPF_Instance;
	arm_biquad_cascade_df2T_instance_f32 BASS_Instance;
	arm_biquad_cascade_df2T_instance_f32 TREBLE_Instance;
	float32_t temp1[TAPE_FREAK_BUFFER_SIZE], temp2[TAPE_FREAK_BUFFER_SIZE];
}TAPE_FREAK_Effect_TypeDef;

// Effect enables
#define TAPE_FREAK_FLAGS_BITCRUSHER_ENABLED		0x01
#define TAPE_FREAK_FLAGS_TONE_CONTROL_ENABLED	0x02
#define TAPE_FREAK_FLAGS_LOFI_FILTER_ENABLED	0x04

extern void Effect_Tape_Freak_Init(uint32_t *effect_s);
extern void Effect_Tape_Freak(uint32_t *effect_s, uint32_t start_sample);

#endif // #ifdef SOUND_ENABLED
#endif /* MODULES_SOUND_EFFECTS_TAPE_FREAK_H_ */
