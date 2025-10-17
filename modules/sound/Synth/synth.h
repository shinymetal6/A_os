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
 * synth.h
 *
 *  Created on: Apr 23, 2025
 *      Author: fil
 */

#ifndef SRC_SYNTH_H_
#define SRC_SYNTH_H_

#ifdef SOUND_ENABLED

#define AUDIO_FAST_RAM		__attribute__((section(".dtcm_user_data"))) __attribute__ ((aligned (16)))

#define SYNTH_BLOCK_SIZE 		SOUND_BLOCK_SIZE         // Number of samples processed per block
#define SYNTH_MAX_VOICES 		16          // Maximum polyphony (number of simultaneous notes)
#define	SYNTH_WAVETABLE_256		256
#define	SYNTH_WAVETABLE_1024	1024
#define	SYNTH_MIDI_NOTES		128

#define	SYNTH_CHANNELS			2


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
    int 		active;               // Flag indicating if the voice is active
    Synth_WaveformType waveform;    // Waveform type
    float 		duty_cycle;         // Duty cycle (0.0 to 1.0)
    const q15_t *wavetable;   // Pointer to custom wavetable for arbitrary waveform
} Synth_Voice_TypeDef;

// MIDI synthesizer state
typedef struct {
	uint8_t				status;
	uint16_t			out_device;		/* for dac is 1 , for codec is 0 */
	uint8_t				active_voices;
	uint8_t				voices_shift;
	int16_t				*out_buf;
	uint32_t 			*effect_s;
	uint8_t				i2s_handle;
	int16_t 			*codec_buf;
	float				sample_rate;
	Synth_Voice_TypeDef voices[SYNTH_MAX_VOICES]; // Polyphonic voices
    uint32_t 			wavetable_size;    //Wavetable size
	void				(*OutFunc)(uint8_t synth_number,int16_t *audio_out,q15_t *audio_in,uint32_t start_sample);

} MidiSynth_TypeDef;
/* status */
#define		SYNTH_ENABLED		0x01
#define		SYNTH_DISABLED		0x00
/* out_device */
//#define		SYNTH_DAC_OUT		32768
#define		SYNTH_DAC_OUT		1
#define		SYNTH_I2S_OUT		0

extern uint8_t Synth_Register(uint8_t channel,MidiSynth_TypeDef *synth);
extern uint8_t Synth_Start(MidiSynth_TypeDef *synth);
extern uint8_t Synth_Stop(MidiSynth_TypeDef *synth);

extern void NoteOn(uint8_t channel,uint8_t note, uint8_t velocity);
extern void NoteOff(uint8_t channel,uint8_t note);
extern void AllNoteOFF(void);

extern void Do_synth(uint8_t synth_number,uint32_t start_sample);

#endif // #ifdef SOUND_ENABLED

#endif /* SRC_SYNTH_H_ */
