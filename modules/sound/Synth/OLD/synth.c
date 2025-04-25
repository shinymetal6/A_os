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
 * synth.c
 *
 *  Created on: Apr 23, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"
#include "synth.h"

// Precomputed sine wavetable (Q15 format)
static q15_t sine_wavetable[WAVETABLE_1024];

// Initialize sine wavetable
void sine_wavetable_init_q15(MidiSynth *synth)
{
    for (int i = 0; i < synth->wavetable_size; i++) {
        float phase = (float)i / (float)synth->wavetable_size; // Normalize phase to [0, 1)
        sine_wavetable[i] = (q15_t)(sin(2.0f * 3.14159265358979323846f * phase) * 32768.0f); // Convert to Q15
    }
}

// Initialize the MIDI synthesizer
void midi_synth_init(MidiSynth *synth, uint32_t tempo, uint32_t ticks_per_beat) {
    synth->current_time = 0;
    synth->tempo = tempo;
    synth->ticks_per_beat = ticks_per_beat;
    synth->samples_per_tick = (uint32_t)((tempo / 1000000.0f) * SAMPLE_RATE / ticks_per_beat);

    // Initialize all voices
    for (int i = 0; i < MAX_VOICES; i++) {
        synth->voices[i].active = 0;
        synth->voices[i].waveform = WAVEFORM_SINE; // Default to sine wave
        synth->voices[i].duty_cycle = 0.5f;       // Default duty cycle (50%)
        synth->voices[i].wavetable = NULL;        // No custom wavetable by default
        synth->voices[i].note = 0;
    }
}

// Load a custom wavetable for arbitrary waveform
void load_custom_wavetable(Voice *voice, const q15_t *wavetable) {
    voice->wavetable = wavetable;
    voice->waveform = WAVEFORM_ARBITRARY;
}

// Note-on event handler
void midi_note_on(MidiSynth *synth, uint8_t note, uint8_t velocity, WaveformType waveform, float duty_cycle) {
    // Find an inactive voice
    for (int i = 0; i < MAX_VOICES; i++) {
        if (!synth->voices[i].active) {
            // Calculate frequency from MIDI note number
            float frequency = 440.0f * powf(2.0f, (note - 69) / 12.0f);
            synth->voices[i].phase = 0.0f;
            synth->voices[i].phase_increment = frequency / SAMPLE_RATE; // Phase increment per sample
            synth->voices[i].amplitude = (q15_t)((velocity / 127.0f) * 32768.0f); // Scale velocity to Q15
            synth->voices[i].waveform = waveform;
            synth->voices[i].duty_cycle = duty_cycle;
            synth->voices[i].active = 1;
            synth->voices[i].note = note;
            break;
        }
    }
}

// Note-off event handler
void midi_note_off(MidiSynth *synth, uint8_t note) {
    for (int i = 0; i < MAX_VOICES; i++) {
        if ((synth->voices[i].active) && (synth->voices[i].note == note)) {
            // Deactivate the voice if it matches the note
            synth->voices[i].active = 0;
        }
    }
}


// Process a block of audio samples
void midi_process_block_q15_256(uint32_t *the_synt, q15_t *output)
{
MidiSynth *synth = (MidiSynth *)the_synt;
    // Clear the output buffer
    memset(output, 0, BLOCK_SIZE * sizeof(q15_t));

    // Process each active voice
    for (int v = 0; v < MAX_VOICES; v++) {
        if (synth->voices[v].active) {
            for (int i = 0; i < BLOCK_SIZE; i++) {
                float phase = synth->voices[v].phase;
                q15_t sample = 0;

                switch (synth->voices[v].waveform) {
                    case WAVEFORM_SINE: {
                        // Adjust phase based on duty cycle
                        float distorted_phase;
                        if (phase < synth->voices[v].duty_cycle) {
                            distorted_phase = phase / synth->voices[v].duty_cycle; // Stretch positive phase
                        } else {
                            distorted_phase = (phase - synth->voices[v].duty_cycle) / (1.0f - synth->voices[v].duty_cycle); // Stretch negative phase
                            distorted_phase += 1.0f; // Shift to [1.0, 2.0]
                        }

                        // Wrap distorted phase to [0, 1)
                        distorted_phase -= (int)distorted_phase;

                        // Map distorted phase to wavetable index
                        float table_index_float = distorted_phase * WAVETABLE_256;
                        uint32_t table_index = (uint32_t)table_index_float % WAVETABLE_256;

                        sample = sine_wavetable[table_index];
                        break;
                    }
                    case WAVEFORM_SQUARE: {
                        // Square wave with adjustable duty cycle
                        sample = (phase < synth->voices[v].duty_cycle) ? 32767 : -32768;
                        break;
                    }
                    case WAVEFORM_SAWTOOTH: {
                        // Sawtooth wave with adjustable duty cycle
                        float value;
                        if (phase < synth->voices[v].duty_cycle) {
                            value = phase / synth->voices[v].duty_cycle; // Stretch rising slope
                        } else {
                            value = (phase - synth->voices[v].duty_cycle) / (1.0f - synth->voices[v].duty_cycle); // Stretch falling slope
                            value -= 1.0f; // Shift to [-1.0, 0.0]
                        }
                        sample = (q15_t)(value * 32768.0f); // Convert to Q15
                        break;
                    }
                    case WAVEFORM_TRIANGLE: {
                        // Triangle wave with adjustable duty cycle
                        float value;
                        if (phase < synth->voices[v].duty_cycle) {
                            value = (phase / synth->voices[v].duty_cycle) * 2.0f - 1.0f; // Rising slope [-1.0, 1.0]
                        } else {
                            value = ((phase - synth->voices[v].duty_cycle) / (1.0f - synth->voices[v].duty_cycle)) * (-2.0f) + 1.0f; // Falling slope [1.0, -1.0]
                        }
                        sample = (q15_t)(value * 32768.0f); // Convert to Q15
                        break;
                    }
                    case WAVEFORM_EXPONENTIAL: {
                        // Exponential wave with adjustable duty cycle
                        float value;
                        if (phase < synth->voices[v].duty_cycle) {
                            value = expf(phase / synth->voices[v].duty_cycle) - 1.0f; // Exponential rise
                        } else {
                            value = -expf((phase - synth->voices[v].duty_cycle) / (1.0f - synth->voices[v].duty_cycle)) + 1.0f; // Exponential fall
                        }
                        sample = (q15_t)(value * 32768.0f); // Convert to Q15
                        break;
                    }
                    case WAVEFORM_ARBITRARY: {
                        // Use custom wavetable
                        if (synth->voices[v].wavetable != NULL) {
                            float table_index_float = phase * (float)synth->wavetable_size;
                            uint8_t table_index = (uint8_t)table_index_float;
                            sample = synth->voices[v].wavetable[table_index];
                        } else {
                            sample = 0; // Silence if no custom wavetable is provided
                        }
                        break;
                    }
                    default:
                        sample = 0; // Default to silence
                        break;
                }

                // Apply amplitude scaling
                sample = (q15_t)(((q31_t)sample * synth->voices[v].amplitude) >> 15);

                // Add voice contribution to output (with clamping to prevent overflow)
                q31_t temp = (q31_t)output[i] + (q31_t)sample;
                if (temp > 32767) temp = 32767;
                if (temp < -32768) temp = -32768;
                output[i] = (q15_t)temp + 32768;

                // Update phase accumulator
                synth->voices[v].phase += synth->voices[v].phase_increment;
                if (synth->voices[v].phase >= 1.0f) { // Wrap around if phase exceeds 1.0
                    synth->voices[v].phase -= 1.0f;
                }
            }
        }
    }
}

// Process a block of audio samples
void midi_process_block_q15_1024(uint32_t *the_synt, q15_t *output)
{
MidiSynth *synth = (MidiSynth *)the_synt;

    // Clear the output buffer
    memset(output, 0, BLOCK_SIZE * sizeof(q15_t));

    // Process each active voice
    for (int v = 0; v < MAX_VOICES; v++) {
        if (synth->voices[v].active) {
            for (int i = 0; i < BLOCK_SIZE; i++) {
                float phase = synth->voices[v].phase;
                q15_t sample = 0;

                switch (synth->voices[v].waveform) {
                    case WAVEFORM_SINE: {
                        // Adjust phase based on duty cycle
                        float distorted_phase;
                        if (phase < synth->voices[v].duty_cycle) {
                            distorted_phase = phase / synth->voices[v].duty_cycle; // Stretch positive phase
                        } else {
                            distorted_phase = (phase - synth->voices[v].duty_cycle) / (1.0f - synth->voices[v].duty_cycle); // Stretch negative phase
                            distorted_phase += 1.0f; // Shift to [1.0, 2.0]
                        }

                        // Wrap distorted phase to [0, 1)
                        distorted_phase -= (int)distorted_phase;

                        // Map distorted phase to wavetable index
                        float table_index_float = distorted_phase * WAVETABLE_1024;
                        uint32_t table_index = (uint32_t)table_index_float % WAVETABLE_1024;

                        sample = sine_wavetable[table_index];
                        break;
                    }
                    case WAVEFORM_SQUARE: {
                        // Square wave with adjustable duty cycle
                        sample = (phase < synth->voices[v].duty_cycle) ? 32767 : -32768;
                        break;
                    }
                    case WAVEFORM_SAWTOOTH: {
                        // Sawtooth wave with adjustable duty cycle
                        float value;
                        if (phase < synth->voices[v].duty_cycle) {
                            value = phase / synth->voices[v].duty_cycle; // Stretch rising slope
                        } else {
                            value = (phase - synth->voices[v].duty_cycle) / (1.0f - synth->voices[v].duty_cycle); // Stretch falling slope
                            value -= 1.0f; // Shift to [-1.0, 0.0]
                        }
                        sample = (q15_t)(value * 32768.0f); // Convert to Q15
                        break;
                    }
                    case WAVEFORM_TRIANGLE: {
                        // Triangle wave with adjustable duty cycle
                        float value;
                        if (phase < synth->voices[v].duty_cycle) {
                            value = (phase / synth->voices[v].duty_cycle) * 2.0f - 1.0f; // Rising slope [-1.0, 1.0]
                        } else {
                            value = ((phase - synth->voices[v].duty_cycle) / (1.0f - synth->voices[v].duty_cycle)) * (-2.0f) + 1.0f; // Falling slope [1.0, -1.0]
                        }
                        sample = (q15_t)(value * 32768.0f); // Convert to Q15
                        break;
                    }
                    case WAVEFORM_EXPONENTIAL: {
                        // Exponential wave with adjustable duty cycle
                        float value;
                        if (phase < synth->voices[v].duty_cycle) {
                            value = expf(phase / synth->voices[v].duty_cycle) - 1.0f; // Exponential rise
                        } else {
                            value = -expf((phase - synth->voices[v].duty_cycle) / (1.0f - synth->voices[v].duty_cycle)) + 1.0f; // Exponential fall
                        }
                        sample = (q15_t)(value * 32768.0f); // Convert to Q15
                        break;
                    }
                    case WAVEFORM_ARBITRARY: {
                        // Use custom wavetable
                        if (synth->voices[v].wavetable != NULL) {
                            float table_index_float = phase * WAVETABLE_1024;
                            uint32_t table_index = (uint32_t)table_index_float % WAVETABLE_1024;
                            sample = synth->voices[v].wavetable[table_index];
                        } else {
                            sample = 0; // Silence if no custom wavetable is provided
                        }
                        break;
                    }
                    default:
                        sample = 0; // Default to silence
                        break;
                }

                // Apply amplitude scaling
                sample = (q15_t)(((q31_t)sample * synth->voices[v].amplitude) >> 15);

                // Add voice contribution to output (with clamping to prevent overflow)
                q31_t temp = (q31_t)output[i] + (q31_t)sample;
                if (temp > 32767) temp = 32767;
                if (temp < -32768) temp = -32768;
                output[i] = (q15_t)temp+32768;

                // Update phase accumulator
                synth->voices[v].phase += synth->voices[v].phase_increment;
                if (synth->voices[v].phase >= 1.0f) { // Wrap around if phase exceeds 1.0
                    synth->voices[v].phase -= 1.0f;
                }
            }
        }
    }
}

MidiSynth *Synth;

ITCM_AREA_CODE uint8_t Synth_Init(MidiSynth *synth)
{
	Synth = synth;
	if ( Synth->wavetable_size == 0 )
	{
		Synth->wavetable_size = WAVETABLE_256;
		Synth->synth = midi_process_block_q15_256;
	}
	if ( Synth->wavetable_size == WAVETABLE_1024 )
	{
		Synth->synth = midi_process_block_q15_1024;
	}
	sine_wavetable_init_q15(Synth);

    // Initialize all voices
    for (int i = 0; i < MAX_VOICES; i++)
    {
        synth->voices[i].active = 0;
        synth->voices[i].waveform = WAVEFORM_SINE; // Default to sine wave
        synth->voices[i].duty_cycle = 0.5f;       // Default duty cycle (50%)
    }
    synth->status = SYNTH_ENABLED;
    return 0;
}

ITCM_AREA_CODE void NoteOn(uint8_t note, uint8_t velocity)
{
	midi_note_on(Synth, note, velocity,WAVEFORM_SINE,0.50F);
}

ITCM_AREA_CODE void NoteOff(uint8_t note)
{
	midi_note_off(Synth, note);
}

ITCM_AREA_CODE void Do_synth(q15_t *output)
{
	HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_SET);
	if ( Synth == NULL )
		return;
	if ( Synth->status ==  SYNTH_ENABLED )
		Synth->synth((uint32_t *)Synth, output);
	HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_RESET);
}

