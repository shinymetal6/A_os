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
 * audio_sources.h
 *
 *  Created on: Oct 22, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_AUDIO_SOURCES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_AUDIO_SOURCES_H_

#define SYNTH_MAX_VOICES 		16          // Maximum polyphony (number of simultaneous notes)
#define SOUND_AUDIO_SOURCES		2           // Maximum sources

// Waveform types
typedef enum {
	SYNTH_WAVEFORM_SINE,
	SYNTH_WAVEFORM_SQUARE,
	SYNTH_WAVEFORM_SAWTOOTH,
	SYNTH_WAVEFORM_TRIANGLE,
	SYNTH_WAVEFORM_EXPONENTIAL,
	SYNTH_WAVEFORM_ARBITRARY
} Synth_WaveformType;

// Voice state for polyphonic synthesis
typedef struct {
	uint8_t		note;
    float 		phase;              // Phase accumulator (floating-point format)
    float 		phase_increment;    // Phase increment per sample (floating-point format)
    q15_t 		amplitude;          // Amplitude (Q15 format)
    int 		active;				// Flag indicating if the voice is active
    Synth_WaveformType waveform;    // Waveform type
    float 		duty_cycle;         // Duty cycle (0.0 to 1.0)
    const q15_t *wavetable;   		// Pointer to custom wavetable for arbitrary waveform
} Synth_Voice_TypeDef;

typedef struct
{
	/* effect header */
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_source;
	uint32_t 			*next_effect;
	q15_t				*in_buf;
	q15_t				*out_buf;
	void 				(*effect)(uint32_t 	*effect_data);
	void 				(*effect_init)(uint32_t *effect_data);
	uint16_t			block_size;
	float				sample_rate;
	/* Source Internals */
	uint8_t				channel;
	uint8_t				channel_in;
	uint8_t				channel_out;
	uint8_t				source_type;
	uint8_t				active_voices;
	uint8_t				voices_shift;
	Synth_Voice_TypeDef voices[SYNTH_MAX_VOICES];	// Polyphonic voices
    uint32_t 			wavetable_size;    			// Wavetable size
    uint32_t			*ptr_gen_struct;
} AUDIO_Source_TypeDef;
/* status */
#define		SOURCE_ENABLED		0x01
#define		SOURCE_DISABLED		0x00

#endif /* MODULES_SOUND_ENGINE_GENERATORS_AUDIO_SOURCES_H_ */
