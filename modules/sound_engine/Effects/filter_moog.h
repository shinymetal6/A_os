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
 * filter_moog.h
 *
 *  Created on: Oct 30, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_EFFECTS_FILTER_MOOG_H_
#define MODULES_SOUND_ENGINE_EFFECTS_FILTER_MOOG_H_

#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/*
    moog_set_params(&filter_l, 1000.0f, 0.8f, 48000.0f);
*/

typedef struct
{
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
	uint32_t			time_start;
	uint32_t			effect_time;
	/* effect data */
	uint16_t			*cutoffFrequency;			// = 1000	Initial cutoff frequency (1 kHz)
	uint16_t			*resonance; 				// = 32768	Resonance (0 to 65535)
	uint16_t			*lfo_rate;					// = 16384	Rate of the LFO [0 .. 65535] easy for user side

    /* Internals */
	float				f_cutoffFrequency;// = 1000.0f; // Initial cutoff frequency (1 kHz)
	float				f_resonance; // = 0.5f;          // Resonance (0.0 to 1.0)
    float 				g;   // cutoff-related gain
    float 				k;   // resonance feedback factor
    float 				y1, y2, y3, y4; // state
} MOOG_F_Effect_TypeDef;

extern	void moog_init(MOOG_F_Effect_TypeDef *f);
extern	void moog_set_params(MOOG_F_Effect_TypeDef *f, float cutoff, float resonance, float sample_rate);
extern	float moog_process(MOOG_F_Effect_TypeDef *f, float input);
extern	void Effect_MOOG_F(uint32_t *effect_s);



#endif /* MODULES_SOUND_ENGINE_EFFECTS_FILTER_MOOG_H_ */
