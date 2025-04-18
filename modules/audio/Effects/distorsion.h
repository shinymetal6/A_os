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
 * distorsion.h
 *
 *  Created on: Apr 18, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_DISTORSION_H_
#define MODULES_AUDIO_EFFECTS_DISTORSION_H_

#define DISTORSION_BLOCK_SIZE 		HALF_NUMBER_OF_AUDIO_SAMPLES         // Number of samples processed per block
#define DISTORSION_PRE_GAIN 		(32767 / 2)         // Number of samples processed per block
#define DISTORSION_MIX		 		(32767 / 2)         // Number of samples processed per block

typedef enum {
    SOFT,
    HARD,
    WAVE_SHAPE,
    NONE
}DISTORSION_EffectType_TypeDef;

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
    q15_t 			gain;     // Pre-gain (amplification before distortion)
    q15_t 			mix;      // Mix between dry and wet signals (0.0 to 1.0)
    q15_t 			temp[DISTORSION_BLOCK_SIZE];
    q15_t 			wet[DISTORSION_BLOCK_SIZE];
	DISTORSION_EffectType_TypeDef 		distorsion_type;
}DISTORSION_Effect_TypeDef;

#endif /* MODULES_AUDIO_EFFECTS_DISTORSION_H_ */
