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
 * wah_filter.h
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_WAH_FILTER_H_
#define MODULES_SOUND_EFFECTS_WAH_FILTER_H_

#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
/*
 * defaults
    wah->attack = 1.0f - expf(-1.0f / (0.0001f * fs));   // ~0.1 ms attack
    wah->release = 1.0f - expf(-1.0f / (0.001f * fs));   // ~1 ms release

    wah->min_cutoff = 300.0f;
    wah->max_cutoff = 2000.0f;
    wah->resonance = 0.85f;
    wah->sensitivity = 1.0f;
*/

typedef struct
{
	uint8_t		status;
	uint8_t		flags;
    uint16_t	*attack;		// units : 1 = 0.1 mSec
    uint16_t	*release;		// units : 1 = 0.1 mSec
    uint16_t	*min_cutoff;	// units : Hz
    uint16_t	*max_cutoff;	// units : Hz
    uint16_t	*resonance;		// units : values 0 - 100
    uint16_t	*sensitivity;	// units : values 2 - 200
    float 		sample_rate;
    /* Internals */
	// Envelope follower
    float env;
    float f_attack;   // ~0.0001 (fast)
    float f_release;  // ~0.001 (slower)

    // Filter
    float g, k;
    float y1, y2, y3, y4;

    // Wah parameters
    float f_min_cutoff;  // Hz (e.g., 300)
    float f_max_cutoff;  // Hz (e.g., 2000)
    float f_resonance;   // 0.7–0.95
    float f_sensitivity; // 0.2–2.0
} WAH_F_Effect_TypeDef;

extern	void Effect_Wah_Init(uint32_t *effect_s);
extern	void Effect_Wah(uint32_t *effect_s, uint32_t start_sample);


#endif /* MODULES_SOUND_EFFECTS_WAH_FILTER_H_ */
