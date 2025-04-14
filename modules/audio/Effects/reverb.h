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
 * reverb.h
 *
 *  Created on: Apr 4, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_REVERB_H_
#define MODULES_AUDIO_EFFECTS_REVERB_H_

#define REVERB_NUM_DELAY_LINES 4
#define REVERB_MAX_DELAY_SAMPLES DEFAULT_SAMPLE_FREQUENCY  // 1 second at the sample frequency


typedef struct {
    float 	*buffer;
    int 	length;
    int 	position;
    float 	feedback;
    float 	gain;
} Reverb_DelayLine_typedef;

typedef struct {
	Reverb_DelayLine_typedef lines[REVERB_NUM_DELAY_LINES];
    float 	mix;
    float 	decay;
} Reverb_effect_internals_typedef;

typedef struct
{
	uint8_t							status;
	uint8_t							initialized;
	uint8_t							flags;
	float 							decayTime;
	float 							mix;
	float 							sampleRate;
	int 							lengths[REVERB_NUM_DELAY_LINES];
	float 							feedbacks[REVERB_NUM_DELAY_LINES];
	float 							gains[REVERB_NUM_DELAY_LINES];
	Reverb_effect_internals_typedef	reverb[REVERB_NUM_DELAY_LINES];
}REVERB_Effect_TypeDef;
#define	REVERB_EFFECT_INITIALIZED	0x01

typedef struct {
    int 	length[REVERB_NUM_DELAY_LINES];
    float 	feedback[REVERB_NUM_DELAY_LINES];
    float 	gain[REVERB_NUM_DELAY_LINES];
	float	decayTime;
	float	mix;
} Reverb_Params_typedef;

extern void 	Do_Reverb(int16_t *inputData, int16_t *outputData, uint8_t index);
extern uint32_t Set_Params_Reverb(int32_t *param_struct, uint8_t index);

#endif /* MODULES_AUDIO_EFFECTS_REVERB_H_ */
