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

#define REVERB_BUFFER_SIZE 			128       // 128 bytes = 64 16-bit integers
#define REVERB_FIXED_ALLPASS_GAIN 	0.5f
#define REVERB_FIXED_ALLPASS_DELAY 	13
#define REVERB_MIX				 	0.5f

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float			buffer[REVERB_BUFFER_SIZE];	// Circular buffer for delay line
	int32_t			read_pos;            // Read position in the buffer
	int32_t			write_pos;           // Write position in the buffer
	float 			comb_gains[6];
	uint8_t 		comb_delays[6];
	float 			allpass_feedback_gain;
	uint8_t 		allpass_delay;
	float 			mix;				// Depth
}REVERB_Effect_TypeDef;


extern void 	Do_Reverb(int16_t *inputData, int16_t *outputData, uint8_t index);

#endif /* MODULES_AUDIO_EFFECTS_REVERB_H_ */
