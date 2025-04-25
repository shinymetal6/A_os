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

#define SAMPLE_RATE 48000      // Sampling rate in Hz
#define BLOCK_SIZE 128         // Number of samples processed per block
#define MAX_VOICES 16          // Maximum polyphony (number of simultaneous notes)
#define	WAVETABLE_256	256
#define	WAVETABLE_1024	1024

// Waveform types
typedef enum {
    WAVEFORM_SINE,
    WAVEFORM_SQUARE,
    WAVEFORM_SAWTOOTH,
    WAVEFORM_TRIANGLE,
    WAVEFORM_EXPONENTIAL,
    WAVEFORM_ARBITRARY
} WaveformType;

// Voice state for polyphonic synthesis
typedef struct {
	uint8_t	note;
    float phase;              // Phase accumulator (floating-point format)
    float phase_increment;    // Phase increment per sample (floating-point format)
    q15_t amplitude;          // Amplitude (Q15 format)
    int active;               // Flag indicating if the voice is active
    WaveformType waveform;    // Waveform type
    float duty_cycle;         // Duty cycle (0.0 to 1.0)
    const q15_t *wavetable;   // Pointer to custom wavetable for arbitrary waveform
} Voice;

// MIDI synthesizer state
typedef struct {
	uint8_t	status;
    Voice voices[MAX_VOICES]; // Polyphonic voices
    uint32_t wavetable_size;    //Wavetable size
    void 		(*synth)(uint32_t *synth, q15_t *output);
    uint32_t current_time;    // Current playback time in samples
    uint32_t tempo;           // Tempo in microseconds per quarter note
    uint32_t ticks_per_beat;  // Ticks per quarter note (from MIDI header)
    uint32_t samples_per_tick;// Samples per MIDI tick
} MidiSynth;
#define		SYNTH_ENABLED		0x01

extern uint8_t Synth_Init(MidiSynth *synth);
extern void NoteOn(uint8_t note, uint8_t velocity);
extern void NoteOff(uint8_t note);
extern void Do_synth(q15_t *output);


#endif /* SRC_SYNTH_H_ */
