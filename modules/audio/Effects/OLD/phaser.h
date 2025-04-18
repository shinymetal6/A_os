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
 * phaser.h
 *
 *  Created on: Apr 7, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_PHASER_H_
#define MODULES_AUDIO_EFFECTS_PHASER_H_

#define PHASER_BUFFER_SIZE		128  	// 128 samples
#define PHASER_SAMPLE_RATE		DEFAULT_SAMPLE_FREQUENCY
#define PHASER_LFO_RATE			0.001F		// LFO frequency in Hz
#define PHASER_LFO_PHASE		0.0		// LFO phase
#define PHASER_DEPTH			0.5		// Depth of modulation
#define PHASER_MIX				0.5		// mix rate

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float 			buffer[PHASER_BUFFER_SIZE];	// Circular buffer for delay line
	int32_t			write_pos;						// Write position in the buffer
	int32_t			read_pos;						// Read position in the buffer
	float 			lfo_phase;						// Phase of the LFO
	float 			lfo_rate;						// Rate of the LFO
	float 			depth;							// Depth of the LFO
	float 			mix;							// Depth of the LFO
}PHASER_Effect_TypeDef;

extern	void Do_Phaser(int16_t *inputData, int16_t *outputData, uint8_t index);


#endif /* MODULES_AUDIO_EFFECTS_PHASER_H_ */
