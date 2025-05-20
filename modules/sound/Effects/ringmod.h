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
 * ringmod.h
 *
 *  Created on: May 17, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_RINGMOD_H_
#define MODULES_SOUND_EFFECTS_RINGMOD_H_

#define RINGMOD_BUFFER_SIZE		SOUND_BLOCK_SIZE  	// 128 samples
#define RINGMOD_SAMPLE_RATE		DEFAULT_SAMPLE_FREQUENCY
#define RINGMOD_DEFAULT_CARRIER_FREQ	800.0f 	// LFO frequency in Hz

typedef struct
{
	float 			carrierFrequency;						// Phase of the LFO
	/* internals */
	float 			carrierPhase;						// Phase of the LFO
	float			phaseIncrement;
}RINGMOD_Effect_TypeDef;

extern void Effect_RingMod(uint32_t *effect_s, uint32_t start_sample);
extern void Effect_RingMod_Init(uint32_t *effect_s);

#endif /* MODULES_SOUND_EFFECTS_RINGMOD_H_ */
