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
 * echo.h
 *
 *  Created on: Apr 25, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_ECHO_H_
#define MODULES_SOUND_EFFECTS_ECHO_H_
#ifdef SOUND_ENABLED

// Constants
#define ECHO_MAX_DELAY_LENGTH DEFAULT_SAMPLE_FREQUENCY/2 // Maximum delay length (1 second at 44.1 kHz)
#define ECHO_BLOCK_SIZE SOUND_BLOCK_SIZE         // Number of samples processed per block
#define ECHO_DELAY_LENGTH_MS 500    // Delay length in milliseconds
#define ECHO_DEFAULT_ATTENUATION 0.5f       // Attenuation factor for the echo
#define ECHO_DEFAULT_DRY_MIX 0.7f           // Dry signal mix level
#define ECHO_DEFAULT_WET_MIX 0.3f           // Wet signal mix level

// Delay line structure
typedef struct
{
    q15_t *buffer; // Circular buffer for delay line
    uint32_t write_index;           // Write index for circular buffer
    uint32_t delay_length;          // Current delay length in samples
} Echo_DelayLine_TypeDef;

typedef struct
{
	uint8_t		status;
	uint8_t		flags;
    uint16_t	*mix;			// 32768 is half effect and half input signal, 65535 is full effect
	float		sample_rate;
    float 		attenuation;    // Attenuation factor
    float 		dry_mix;        // Dry signal mix level
    float 		wet_mix;        // Wet signal mix level
    /* Internals */
    Sound_DelayLine_TypeDef delay_line; // Delay line
	uint32_t	delay_ms;
} Echo_Effect_TypeDef;

extern void Effect_Echo_Init(uint32_t *effect_s);
extern void Effect_Echo(uint32_t *effect_s, uint32_t start_sample);

#endif // #ifdef SOUND_ENABLED

#endif /* MODULES_SOUND_EFFECTS_ECHO_H_ */
