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
 * chorus.h
 *
 *  Created on: Apr 25, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_CHORUS_H_
#define MODULES_SOUND_EFFECTS_CHORUS_H_
#ifdef SOUND_ENABLED

#define CHORUS_BLOCK_SIZE 		SOUND_BLOCK_SIZE         // Number of samples processed per block
#define CHORUS_MAX_DELAY_LENGTH DEFAULT_SAMPLE_FREQUENCY // Maximum delay length (1 second at 44.1 kHz)
#define CHORUS_LFO_FREQUENCY 	10     // LFO frequency in Hz
#define CHORUS_DEFAULT_MIX_DRY	0.5f           // Dry signal mix level
#define CHORUS_DEFAULT_MIX_WET	0.5f           // Wet signal mix level

typedef struct {
    q15_t buffer[CHORUS_MAX_DELAY_LENGTH]; // Circular buffer for delay line
    uint32_t write_index;           // Write index for circular buffer
    uint32_t delay_length;          // Current delay length in samples
} Chorus_DelayLine_TypeDef;

// Chorus state
typedef struct
{
	uint8_t						status;
	uint8_t						flags;
    uint16_t					*mix;			// 32768 is half effect and half input signal, 65535 is full effect
	float						sample_rate;
	/* Internals */
	Chorus_DelayLine_TypeDef	*delay_line; // Delay line
    q15_t 						lfo_phase;      // LFO phase in Q15 format
    float 						dry_mix;        // Dry signal mix level
    float 						wet_mix;        // Wet signal mix level
} Chorus_Effect_TypeDef;

extern void Effect_Chorus_Init(uint32_t *effect_s);
extern void Effect_Chorus(uint32_t *effect_s, uint32_t start_sample);


#endif // #ifdef SOUND_ENABLED

#endif /* MODULES_SOUND_EFFECTS_CHORUS_H_ */
