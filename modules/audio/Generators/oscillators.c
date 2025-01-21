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
 * oscillators.c
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
//#include "../../../kernel/kernel_opt.h"

#include "../audio.h"
#include "../effects.h"

#ifdef AUDIO_GENERATORS_ENABLED

#include "oscillators.h"
#include "oscillator_core.h"

extern	AudioFlagsTypeDef		AudioFlags;

AUDIO_FAST_RAM	__attribute__ ((aligned (16))) OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];

ITCM_AREA_CODE	uint32_t FindOscillatorByMidiNote(uint8_t midi_note)
{
uint32_t	i;
	for(i=0;i<NUMOSCILLATORS;i++)
		if ( Oscillator[i].midi_note == midi_note )
			return i;
	return NUMOSCILLATORS+1;
}

ITCM_AREA_CODE	uint32_t FindFreeOscillator(void)
{
uint32_t		i;
uint8_t			osc_ret = 0;
uint32_t		oldest_osc=0;

	for(i=0;i<NUMOSCILLATORS;i+=VOICES)
	{
		if ( (Oscillator[i].state & OSCILLATOR_ON ) != OSCILLATOR_ON )
		{
			return i;
		}
		else
		{
			if ( Oscillator[i].oscillator_age > oldest_osc )
			{
				oldest_osc = Oscillator[i].oscillator_age;
				osc_ret = i;
			}
		}
	}
	oldest_osc=0;
	return osc_ret;
}

ITCM_AREA_CODE	void NoteOFF( uint16_t midi_note , uint8_t velocity)
{
uint8_t	i;
	for(i=0;i<NUMOSCILLATORS;i++)
	{
		if (( Oscillator[i].midi_note == midi_note ) && ((Oscillator[i].state & OSCILLATOR_ON ) == OSCILLATOR_ON ))
		{
			Oscillator[i].state |= OSCILLATOR_GO_OFF;
		}
	}
}

ITCM_AREA_CODE	void DisableAllOscillator(void)
{
uint8_t	i;
	for(i=0;i<NUMOSCILLATORS;i++)
		Oscillator[i].state &= ~OSCILLATOR_ON;
}

extern	float		midi_freq[128];
extern	uint32_t	sample_frequency;

ITCM_AREA_CODE	void NoteON(uint16_t midi_note , uint8_t velocity)
{
float	delta_phase;
float	freq;
uint32_t	osc_number,i;

	osc_number = FindFreeOscillator();
	for(i=0 ; i< VOICES; i++)
	{
		Oscillator[osc_number].midi_note = midi_note;
		freq = midi_freq[Oscillator[osc_number].midi_note] + Oscillator[osc_number].detune;
		delta_phase = (float )WAVETABLE_SIZE / ((float )(sample_frequency / 2 ) / freq);
		Oscillator[osc_number+i].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
		Oscillator[osc_number+i].current_phase = 0;
		Oscillator[osc_number+i].midi_note = midi_note;
		Oscillator[osc_number+i].velocity = velocity;
		Oscillator[osc_number+i].state = OSCILLATOR_ON;
		Oscillator[osc_number+i].oscillator_age = 0;
		Oscillator[osc_number+i].volume = ((float )1.0F);
	}
}

void InitOscillators(void)
{
uint16_t	i;

	sample_frequency = DEFAULT_SAMPLE_FREQUENCY;
	for(i=0;i<NUMOSCILLATORS;i++)
	{
		Oscillator[i].current_phase = 0.0f;
		Oscillator[i].detune = 0.0f;
		Oscillator[i].state &= ~OSCILLATOR_ON;
		Oscillator[i].waveform = SINE;
		Oscillator[i].midi_note = INVALID_MIDI_NOTE;
		Oscillator[i].duty = 50*DUTY_SCALE;
		Oscillator[i].volume = 1.0F;
		Oscillator[i].delta_phase = 0;
		Oscillator[i].state = 0;
	}
	InitOscillatorsTables();
}

uint8_t Freq2Midi(float freq )
{
uint8_t	i;
	for(i=0;i<MIDI_NOTES;i++)
	{
		if ((midi_freq[i]>=freq) && (midi_freq[i+1]<=freq))
			return i;
	}
	return 255;
}

float Midi2Freq(uint8_t midi_note )
{
	if ( midi_note < MIDI_NOTES)
		return midi_freq[midi_note];
	return 0;
}

void SetSampleFrequency(uint32_t new_sample_frequency)
{
	sample_frequency = new_sample_frequency;

}
#endif // #ifdef AUDIO_GENERATORS_ENABLED

