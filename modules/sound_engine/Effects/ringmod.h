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
 * ringmod.h
 *
 *  Created on: May 17, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_RINGMOD_H_
#define MODULES_SOUND_EFFECTS_RINGMOD_H_

#define RINGMOD_BUFFER_SIZE		SOUND_BLOCK_SIZE  	// 128 samples
#define RINGMOD_DEFAULT_CARRIER_FREQ	800.0F 	// LFO frequency in Hz
#define RINGMOD_MAX_CARRIER_FREQ		4800.0F 	// LFO max frequency in Hz

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
	uint16_t			*carrierFrequency;					// Phase of the LFO
	/* internals */
	float 				f_carrierFrequency;					// Phase of the LFO
	float 				carrierPhase;						// Phase of the LFO
	float				phaseIncrement;
	q15_t				(*ringmod_effect)(uint32_t *ringmod,float input );
}RINGMOD_Effect_TypeDef;
#define	RINGMODE_STATUS_INTERNAL	0x01
#define	RINGMODE_STATUS_EXTERNAL	0x02
extern void Effect_RingMod(uint32_t *effect_s);
extern void Effect_RingMod_Init(uint32_t *effect_s);

#endif /* MODULES_SOUND_EFFECTS_RINGMOD_H_ */
