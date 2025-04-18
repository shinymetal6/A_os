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
 * moog.h
 *
 *  Created on: Apr 18, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_MOOG_H_
#define MODULES_AUDIO_EFFECTS_MOOG_H_

#define MOOG_BLOCK_SIZE 		HALF_NUMBER_OF_AUDIO_SAMPLES         // Number of samples processed per block
#define MOOG_SAMPLE_RATE		DEFAULT_SAMPLE_FREQUENCY
#define	MOOG_DEFAULT_CUTOFF		(q15_t)(0.1f * 32768.0f)
#define	MOOG_DEFAULT_RESONANCE	(q15_t)(0.5f * 32768.0f);

// Moog filter state variables
typedef struct {
    q15_t z1, z2, z3, z4;      // State variables for the filter stages
} MOOG_Filter_State_TypeDef;

// Moog filter parameters
typedef struct {
    q15_t cutoff_freq;         // Cutoff frequency in Q15 format
    q15_t resonance;           // Resonance in Q15 format
} MOOG_Filter_Params_TypeDef;

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float32_t 		cutoff_freq;
	float32_t 		resonance;
	MOOG_Filter_State_TypeDef 		MOOG_Filter_State[1];
	MOOG_Filter_Params_TypeDef 		MOOG_Filter_Params[1];
}MOOG_Effect_TypeDef;

typedef struct
{
	float32_t 		cutoff_freq;
	float32_t 		resonance;
}MOOG_ParamsChange_Effect_TypeDef;


extern void Do_Moog(int16_t *inputData, int16_t *outputData, uint8_t index);
extern void ParamsEffectSet_Moog(uint32_t *param_struct, uint8_t index);


#endif /* MODULES_AUDIO_EFFECTS_MOOG_H_ */
