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
 * fm_oscillator.h
 *
 *  Created on: Apr 18, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_GENERATORS_FM_OSCILLATOR_H_
#define MODULES_AUDIO_GENERATORS_FM_OSCILLATOR_H_

#define FM_OSC_BUFFER_SIZE 		HALF_NUMBER_OF_AUDIO_SAMPLES
#define FM_OSC_SAMPLE_RATE		DEFAULT_SAMPLE_FREQUENCY

// Waveform types
typedef enum {
    WAVEFORM_SINE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH
} WaveformType;

// Oscillator state variables
typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
	float32_t 		frequency;
    WaveformType	waveform;    // Waveform type
    q31_t			phase;              // Phase accumulator (Q31 format for high precision)
    q31_t			phase_increment;    // Phase increment per sample (Q31 format)
} FM_Oscillator_typedef;

#endif /* MODULES_AUDIO_GENERATORS_FM_OSCILLATOR_H_ */
