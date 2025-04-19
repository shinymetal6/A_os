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
 *  Created on: Apr 18, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_REVERB_H_
#define MODULES_AUDIO_EFFECTS_REVERB_H_

#define REVERB_BUFFER_SIZE 			HALF_NUMBER_OF_AUDIO_SAMPLES
#define REVERB_NUM_COMB 			4
#define REVERB_NUM_ALLPASS 			2
#define REVERB_SAMPLE_RATE			DEFAULT_SAMPLE_FREQUENCY

#define REVERB_SMALL_ROOM			0.5F
#define REVERB_MEDIUM_ROOM			1.0F
#define REVERB_LARGE_ROOM			1.5F
#define REVERB_HALL_ROOM			2.0F

typedef struct {
    q15_t buffer[REVERB_BUFFER_SIZE*2]; // Delay line buffer
    int write_index;          // Write index for circular buffer
    int delay_samples;        // Delay in samples
    q15_t feedback;           // Feedback gain
} REVERB_CombFilter_TypeDef;

// All-pass filter structure
typedef struct {
    q15_t buffer[REVERB_BUFFER_SIZE*2]; // Delay line buffer
    int write_index;          // Write index for circular buffer
    int delay_samples;        // Delay in samples
} REVERB_AllPassFilter_TypeDef;

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float 			room_size;			//Small Room 0.5 , Medium Room 1.0 , Large Room 1.5 , Cathedral/Hall 2.0
	float 			mix;				// Depth
	REVERB_CombFilter_TypeDef		REVERB_CombFilter[REVERB_NUM_COMB];
	REVERB_AllPassFilter_TypeDef	REVERB_AllPassFilter[REVERB_NUM_ALLPASS];
    float32_t base_delays[REVERB_NUM_COMB];// = {0.0297f, 0.0371f, 0.0411f, 0.0437f}; // Base delay times in seconds
    float32_t feedback_gains[REVERB_NUM_COMB];// = {0.84f, 0.84f, 0.84f, 0.84f};       // Feedback gains

}REVERB_Effect_TypeDef;


extern void 	Do_Reverb(int16_t *inputData, int16_t *outputData, uint8_t index);

#endif /* MODULES_AUDIO_EFFECTS_REVERB_H_ */
