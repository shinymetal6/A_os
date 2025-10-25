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
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"
#include "synth.h"

/* This cannot be in AUDIO_FAST_RAM unless not initialized by user */
/* So the better solution is to have this in std boot-initialized ram */
extern	AUDIO_Source_TypeDef *AudioSource[2];

// Precomputed sine wavetable (Q15 format)
AUDIO_FAST_RAM 	static q15_t sine_wavetable[SYNTH_WAVETABLE_1024];
__attribute__((section(".table"))) __attribute__ ((aligned (32))) const float	rom_midi_freq[SYNTH_MIDI_NOTES] =
{
		8.176,
		8.662,
		9.177,
		9.723,
		10.301,
		10.913,
		11.562,
		12.250,
		12.978,
		13.750,
		14.568, /* 10 */
		15.434,
		16.352,
		17.324,
		18.354,
		19.445,
		20.601,
		21.826,
		23.124,
		24.499,
		25.956, /* 20 */
		27.500,
		29.135,
		30.867,
		32.703,
		34.648,
		36.708,
		38.890,
		41.203,
		43.653,
		46.249, /* 30 */
		48.999,
		51.913,
		55.000,
		58.270,
		61.735,
		65.406,
		69.295,
		73.416,
		77.781,
		82.406, /* 40 */
		87.307,
		92.499,
		97.998,
		103.82,
		110.00,
		116.54,
		123.47,
		130.81,
		138.59,
		146.83, /* 50 */
		155.56,
		164.81,
		174.61,
		184.99,
		195.99,
		207.65,
		220.00,
		233.08,
		246.94,
		261.63, /* 60 */
		277.18,
		293.66,
		311.13,
		329.63,
		349.23,
		369.99,
		391.99,
		415.31,
		440.00,
		466.16, /* 70 */
		489.88,
		523.25,
		554.37,
		587.33,
		622.25,
		659.26,
		698.46,
		739.99,
		783.99,
		830.61, /* 80 */
		880.00,
		932.32,
		987.77,
		1046.5,
		1108.7,
		1174.7,
		1244.5,
		1318.5,
		1396.9,
		1480.0, /* 90 */
		1568.0,
		1661.2,
		1760.0,
		1864.7,
		1975.5,
		2093.0,
		2217.5,
		2349.3,
		2489.0,
		2637.0, /* 100 */
		2793.8,
		2960.0,
		3136.0,
		3322.4,
		3520.0,
		3729.3,
		3951.1,
		4186.0,
		4434.9,
		4698.6, /* 110 */
		4978.0,
		5274.0,
		5587.7,
		5919.9,
		6271.9,
		6644.9,
		7040.0,
		7458.6,
		7902.1,
		8372.0, /* 120 */
		8869.8,
		9397.3,
		9956.1,
		10548.1,
		11175.3,
		11839.8,
		12543.9 /* 127 */
};
AUDIO_FAST_RAM	float	midi_freq[SYNTH_MIDI_NOTES];

// Initialize sine wavetable
ITCM_AREA_CODE static void synth_sine_wavetable_init(AUDIO_Source_TypeDef *synth)
{
    for (int i = 0; i < synth->wavetable_size; i++) {
        float phase = (float)i / (float)synth->wavetable_size; // Normalize phase to [0, 1)
        sine_wavetable[i] = (q15_t)(sin(2.0f * 3.14159265358979323846f * phase) * 32768.0f); // Convert to Q15
    }
}

ITCM_AREA_CODE static uint8_t synth_calc_shift(uint8_t voices)
{
	switch(voices)
	{
	case	0:
	case	1:	return 0;
	case	2:	return 1;
	case	3:
	case	4:
	case	5:
	case	6:	return 2;
	default:	return 3;
	}
}
// Note-on event handler
ITCM_AREA_CODE static void synth_note_on(AUDIO_Source_TypeDef *synth, uint8_t note, uint8_t velocity, Synth_WaveformType waveform, float duty_cycle)
{
    // Find an inactive voice
    for (int i = 0; i < SYNTH_MAX_VOICES; i++)
    {
        if (!synth->voices[i].active)
        {
            synth->voices[i].phase = 0.0f;
            synth->voices[i].phase_increment = midi_freq[note] / (synth->sample_rate*8.0F); // Phase increment per sample
            synth->voices[i].amplitude = (q15_t)((velocity / 127.0f) * 32768.0f); // Scale velocity to Q15 , max val = 127
            synth->voices[i].waveform = waveform;
            synth->voices[i].duty_cycle = duty_cycle;
            synth->voices[i].active = 1;
            synth->voices[i].note = note;
            synth->active_voices++;
            synth->voices_shift = synth_calc_shift(synth->active_voices);
            break;
        }
    }
}

// Note-off event handler
ITCM_AREA_CODE static void synth_note_off(AUDIO_Source_TypeDef *synth, uint8_t note)
{
    for (int i = 0; i < SYNTH_MAX_VOICES; i++) {
        if ((synth->voices[i].active) && (synth->voices[i].note == note)) {
            // Deactivate the voice if it matches the note
            synth->voices[i].active = 0;
            synth->active_voices--;
            synth->voices_shift = synth_calc_shift(synth->active_voices);
        }
    }
}

// all-note-off event handler
ITCM_AREA_CODE static void synth_all_note_off(AUDIO_Source_TypeDef *synth)
{
    for (int i = 0; i < SYNTH_MAX_VOICES; i++)
    {
    	synth->voices[i].active = 0;
    }
    synth->active_voices = 0;
    synth->voices_shift = synth_calc_shift(0);
}
// Process a block of audio samples
ITCM_AREA_CODE void Synth_Process_Block(uint32_t *the_synt)
{
AUDIO_Source_TypeDef *synth = (AUDIO_Source_TypeDef *)the_synt;

q15_t *output;

	output = synth->work_buf; // no, so is half buffer

    // Clear the output buffer
    memset(output, 0, synth->block_size * sizeof(q15_t));

    // Process each active voice
    for (int v = 0; v < SYNTH_MAX_VOICES; v++) {
        if (synth->voices[v].active) {
            for (int i = 0; i < synth->block_size; i++) {
                float phase = synth->voices[v].phase;
                q15_t sample = 0;

                switch (synth->voices[v].waveform) {
                    case SYNTH_WAVEFORM_SINE: {
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
                        float table_index_float;
                        uint32_t table_index;
                        if ( synth->wavetable_size == SYNTH_WAVETABLE_256 )
                        {
                            table_index_float = distorted_phase * SYNTH_WAVETABLE_256;
                            table_index = (uint32_t)table_index_float % SYNTH_WAVETABLE_256;
                        }
                        else
                        {
                            table_index_float = distorted_phase * SYNTH_WAVETABLE_1024;
                            table_index = (uint32_t)table_index_float % SYNTH_WAVETABLE_1024;
                        }

                        sample = sine_wavetable[table_index];
                        break;
                    }
                    case SYNTH_WAVEFORM_SQUARE: {
                        // Square wave with adjustable duty cycle
                        sample = (phase < synth->voices[v].duty_cycle) ? 32767 : -32768;
                        break;
                    }
                    case SYNTH_WAVEFORM_SAWTOOTH: {
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
                    case SYNTH_WAVEFORM_TRIANGLE: {
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
                    case SYNTH_WAVEFORM_EXPONENTIAL: {
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
                    case SYNTH_WAVEFORM_ARBITRARY: {
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

                // Add voice contribution to output
                q31_t temp = (q31_t)(output[i]) + (q31_t)(sample);
                temp >>= (q31_t)synth->voices_shift;
                output[i] = (q15_t)temp;

                // Update phase accumulator
                synth->voices[v].phase += synth->voices[v].phase_increment;
                if (synth->voices[v].phase >= 1.0f)	// Wrap around if phase exceeds 1.0
                    synth->voices[v].phase -= 1.0f;
            }
        }
    }
}

ITCM_AREA_CODE uint8_t Synth_Start(AUDIO_Source_TypeDef *synth)
{
    synth->status = SOURCE_ENABLED;
    return 0;
}

ITCM_AREA_CODE uint8_t Synth_Stop(AUDIO_Source_TypeDef *synth)
{
    synth->status &= ~SOURCE_DISABLED;
    return 0;
}

// Load a custom wavetable for arbitrary waveform
ITCM_AREA_CODE void synth_load_custom_wavetable(Synth_Voice_TypeDef *voice, const q15_t *wavetable)
{
    voice->wavetable = wavetable;
    voice->waveform = SYNTH_WAVEFORM_ARBITRARY;
}

ITCM_AREA_CODE void NoteOn(uint8_t channel,uint8_t note, uint8_t velocity)
{
	if ( channel >= SYNTH_CHANNELS )
		return;
	AUDIO_Source_TypeDef *synth = AudioSource[channel];
	if ( synth == NULL )
		return;
	synth_note_on(synth, note, velocity,SYNTH_WAVEFORM_SINE,0.50F);
}

ITCM_AREA_CODE void NoteOff(uint8_t channel,uint8_t note)
{
	if ( channel >= SYNTH_CHANNELS )
		return;
	AUDIO_Source_TypeDef *synth = AudioSource[channel];
	if ( synth == NULL )
		return;
	synth_note_off(synth, note);
}

ITCM_AREA_CODE void AllNoteOFF(void)
{
AUDIO_Source_TypeDef *synth;
	synth = AudioSource[0];
	if ( synth == NULL )
		return;
	synth_all_note_off(synth);
	synth = AudioSource[1];
	if ( synth == NULL )
		return;
	synth_all_note_off(synth);
}

extern	uint8_t number_of_synths;

ITCM_AREA_CODE uint8_t Synth_Register(uint8_t channel,AUDIO_Source_TypeDef *synth)
{
uint32_t	i;

	if ( synth->out_buf == NULL )
		return 1;
	if ( synth->work_buf == NULL )
			return 1;
	if ( channel >= SYNTH_CHANNELS )
		return 1;
	for(i=0;i<SYNTH_MIDI_NOTES;i++)
		midi_freq[i] =	rom_midi_freq[i];
	AudioSource[channel] = synth;
	if ( synth->wavetable_size == 0 )
		synth->wavetable_size = SYNTH_WAVETABLE_1024;
	if ( synth->sample_rate == 0 )
		synth->sample_rate = Sound_Sample_Frequency;
	synth_sine_wavetable_init(synth);
	synth->active_voices = 0;
	/*
	if ( synth->out_device == SOURCE_TO_I2S_OUT)
		synth->OutFunc = synth_to_i2s_out;
	else
		synth->OutFunc = synth_to_dac_out;
	*/
	synth->source_type = SOUND_SOURCE_IS_SYNTH;
	// Initialize all voices
	for (int i = 0; i < SYNTH_MAX_VOICES; i++)
	{
		synth->voices[i].active = 0;
		synth->voices[i].waveform = SYNTH_WAVEFORM_SINE; // Default to sine wave
		synth->voices[i].duty_cycle = 0.5f;       // Default duty cycle (50%)
		synth->voices[i].wavetable = NULL;        // No custom wavetable by default
		synth->voices[i].note = 0;
	}
	number_of_synths++;
	return 0;
}

#endif // #ifdef SOUND_ENABLED


