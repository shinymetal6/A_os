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
 * flanger.h
 *
 *  Created on: Apr 18, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_FLANGER_H_
#define MODULES_AUDIO_EFFECTS_FLANGER_H_

#define FLANGER_BUFFER_SIZE		HALF_NUMBER_OF_AUDIO_SAMPLES  	// 128 samples
#define FLANGER_SAMPLE_RATE		DEFAULT_SAMPLE_FREQUENCY
#define FLANGER_MAX_DELAY_MS 	20       // Maximum delay in milliseconds
typedef struct {
    float 	buffer[FLANGER_BUFFER_SIZE];
    int 	write_index;
    int 	max_delay_samples;
} FLANGER_DelayLine_TypeDef;

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float 			buffer[FLANGER_BUFFER_SIZE];	// Circular buffer for delay line
	uint32_t		write_pos;					// Write position in the buffer
	uint32_t		read_pos;					// Read position in the buffer
    float 			lfo_frequency; 				// LFO frequency in Hz
    float 			lfo_phase;
    float 			lfo_value;
    float 			feedback;      				// Feedback amount (0.0 to 1.0)
    float 			mix;		           		// Mix between dry and wet signals (0.0 to 1.0)
    FLANGER_DelayLine_TypeDef	delay_line[1];
}FLANGER_Effect_TypeDef;

extern void Do_Flanger(int16_t *inputData, int16_t *outputData, uint8_t index);

#endif /* MODULES_AUDIO_EFFECTS_FLANGER_H_ */
