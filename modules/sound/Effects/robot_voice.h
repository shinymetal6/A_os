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

typedef struct {
    // Ring modulator
    float ring_phase;
    float ring_freq;        // Hz (300–1000)
    uint8_t ring_wave;      // 0=sine, 1=square

    // Bit crusher
    float bit_depth;        // 4–12 bits
    float sample_rate;      // e.g., 48000

    // Bandpass filter (simple 2-pole)
    float bp_x1, bp_x2;
    float bp_y1, bp_y2;
    float bp_freq;          // center freq (Hz)
    float bp_q;             // Q factor (0.5–2.0)
} ROBOT_VOICE_Effect_TypeDef;

void robot_init(ROBOT_VOICE_Effect_TypeDef *rv, float sample_rate);
void robot_set_params(ROBOT_VOICE_Effect_TypeDef *rv, float ring_freq, uint8_t ring_wave, float bit_depth, float bp_freq, float bp_q);
float robot_process(ROBOT_VOICE_Effect_TypeDef *rv, float input);

#endif /* MODULES_SOUND_EFFECTS_ROBOT_VOICE_H_ */
