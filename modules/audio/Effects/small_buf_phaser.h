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
 * small_buf_phaser.h
 *
 *  Created on: Apr 14, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_SMALL_BUF_PHASER_H_
#define MODULES_AUDIO_EFFECTS_SMALL_BUF_PHASER_H_


#define SMALL_BUF_PHASER_PI 			3.14159265358979323846
#define SMALL_BUF_PHASER_SAMPLE_RATE	44100 // Sample rate in Hz
#define SMALL_BUF_PHASER_MAX_DELAY 		HALF_NUMBER_OF_AUDIO_SAMPLES     // Small buffer size (in samples) = 128
#define SMALL_BUF_PHASER_NUM_STAGES 	2      // Fewer stages for simplicity

// All-pass filter structure
typedef struct {
    float buffer[MAX_DELAY];
    int write_pos;
} AllPassFilter_typedef;


typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float 			lfo_rate,depth,feedback;
	Phaser_TypeDef	*phaser;
}SmallBuf_PHASER_Effect_TypeDef;

#endif /* MODULES_AUDIO_EFFECTS_SMALL_BUF_PHASER_H_ */
