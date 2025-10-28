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
 * space_echo.h
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_SPACE_ECHO_H_
#define MODULES_SOUND_EFFECTS_SPACE_ECHO_H_

#ifndef SPACE_ECHO_H
#define SPACE_ECHO_H

#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define ECHO_MAX_DELAY_MS 	800   // Max delay time
#define ECHO_MAX_FEEDBACK 	85.0F    //
#define ECHO_BUFFER_SIZE (ECHO_MAX_DELAY_MS * 48) // 48 kHz → ~38,400 samples

#define	ECHO_DEFAULT_DELAY_TIME	300.0F
#define	ECHO_DEFAULT_FEEDBACK	0.5F
#define	ECHO_DEFAULT_LFO_RATE	1.2F
#define	ECHO_DEFAULT_LFO_DEPTH	3.0F
#define	ECHO_DEFAULT_CUTOFF		1200.0F

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
	/* effect data */
    uint16_t 			*delay_time_ms;        // 50–800 ms
    uint16_t 			*feedback;             // 0.0–0.8 multiplied by 100 , so 0 to 80
    uint16_t 			*lfo_rate;             // 0.5–3 Hz multiplied by 10, so 5 to 30
    uint16_t 			*lfo_depth_ms;         // 0–10 ms
    uint16_t 			*cutoff;               // LPF on feedback (200–5000 Hz)
    /* Internals */

	// Delay buffer (mono for now; duplicate for stereo)
    float 				buffer[ECHO_BUFFER_SIZE];
    uint32_t 			write_ptr;

    // Parameters
    float 				f_delay_time_ms;        // 50–800 ms
    float 				f_feedback;             // 0.0–0.8
    float 				f_lfo_rate;             // 0.5–3 Hz
    float 				f_lfo_depth_ms;         // 0–10 ms
    float 				f_cutoff;               // LPF on feedback (200–5000 Hz)

    // LFO state
    float 				lfo_phase;

    // Moog filter state (for feedback path)
    float 				g, k;
    float 				y1, y2, y3, y4;
} SPACE_ECHO_Effect_TypeDef;

extern void Effect_Space_Echo_Init(uint32_t *effect_s);
extern void Effect_Space_Echo(uint32_t *effect_s);

#endif

#endif /* MODULES_SOUND_EFFECTS_SPACE_ECHO_H_ */
