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
 * chorus.h
 *
 *  Created on: Apr 18, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_CHORUS_H_
#define MODULES_AUDIO_EFFECTS_CHORUS_H_

#define CHORUS_BUFFER_SIZE		HALF_NUMBER_OF_AUDIO_SAMPLES  	// 128 samples
#define CHORUS_SAMPLE_RATE		DEFAULT_SAMPLE_FREQUENCY
#define CHORUS_MAX_DELAY_MS		50

typedef struct
{
	uint8_t		status;
	uint8_t		initialized;
	uint8_t		flags;
    float 		lfo_frequency; 				// LFO frequency in Hz
    float 		feedback;
    float 		mix;		           		// Mix between dry and wet signals (0.0 to 1.0)
    float 		lfo_phase;
    float 		lfo_value;
    float 		buffer[CHORUS_BUFFER_SIZE];
    uint32_t	write_index;
    uint32_t	max_delay_samples;
}CHORUS_Effect_TypeDef;

extern void Do_Chorus(int16_t *inputData, int16_t *outputData, uint8_t index);

#endif /* MODULES_AUDIO_EFFECTS_CHORUS_H_ */
