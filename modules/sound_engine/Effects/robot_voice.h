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
 * robot_voice.h
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_ROBOT_VOICE_H_
#define MODULES_SOUND_EFFECTS_ROBOT_VOICE_H_

#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define	ROBOT_VOICE_SQUARE_WAVE	1
#define	ROBOT_VOICE_SINE_WAVE	0

#define	ROBOT_VOICE_RING_FREQ_MIN	100.0F
#define	ROBOT_VOICE_RING_FREQ_MAX	2000.0F

//600.0f, 1, 8.0f, 1200.0f, 1.2f); // default: square wave, 8-bit, 1.2kHz BP
typedef struct {
	/* effect header */
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_effect;
	q15_t				*in_buf;
	q15_t				*out_buf;
	int16_t				*device_out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint16_t			block_size;
	float				sample_rate;
	uint8_t				in_device;
	uint8_t				out_device;
	uint8_t				channel_in,channel_out;
	/* effect data */
    uint16_t			*ring_freq;	// Hz (300–1000)
    uint16_t			*ring_wave;	// 0=sine, 1=square , default square
    uint16_t			*bit_depth;	// default 8 bit
    uint16_t			*bp_freq;	// default 1.2KHz , in Hz unit
    uint16_t			*bp_q;	 	// default 1.2 , in 0.1 unit

    //float ring_freq, uint8_t ring_wave, float bit_depth, float bp_freq, float bp_q)
	/* Internals */
	// Ring modulator
    float 				f_ring_freq;      // Hz (300–1000)
    float 				f_ring_phase;
    uint8_t				i_ring_wave;      // 0=sine, 1=square

    // Bit crusher
    float 				f_bit_depth;        // 4–12 bits

    // Bandpass filter (simple 2-pole)
    float 				f_bp_x1, f_bp_x2;
    float 				f_bp_y1, f_bp_y2;
    float 				f_bp_freq;          // center freq (Hz)
    float 				f_bp_q;             // Q factor (0.5–2.0)
} ROBOT_VOICE_Effect_TypeDef;

extern	void Effect_Robot_Init(uint32_t *effect_s);
extern	void Effect_Robot(uint32_t *effect_s);

#endif /* MODULES_SOUND_EFFECTS_ROBOT_VOICE_H_ */
