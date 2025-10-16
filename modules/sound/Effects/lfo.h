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
 * lfo.h
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_LFO_H_
#define MODULES_SOUND_EFFECTS_LFO_H_

#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

typedef enum {
    LFO_SINE,
    LFO_TRIANGLE,
    LFO_SAW,
    LFO_SQUARE
} lfo_waveform_t;

typedef struct
{
	float			sample_rate;
	uint16_t		*rate;
	uint16_t		*depth;
    lfo_waveform_t 	waveform;
/* internal */
    float			phase;            // 0.0 to 1.0
    float			phase_inc;        // per-sample phase increment
    float			f_rate;             // Hz
    float			f_depth;            // 0.0 to 1.0 (modulation depth)
} LFO_Effect_TypeDef;

void Effect_LFO_init(LFO_Effect_TypeDef *lfo, float sample_rate);
void lfo_set_rate(LFO_Effect_TypeDef *lfo, float rate_hz);
void lfo_set_depth(LFO_Effect_TypeDef *lfo, float depth); // 0.0–1.0
void lfo_set_waveform(LFO_Effect_TypeDef *lfo, lfo_waveform_t wf);
float Effect_LFO_process(LFO_Effect_TypeDef *lfo); // returns [-1.0, 1.0]


#endif /* MODULES_SOUND_EFFECTS_LFO_H_ */
