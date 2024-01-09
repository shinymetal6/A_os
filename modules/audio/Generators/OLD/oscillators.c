/*
 * oscillators.c
 *
 *  Created on: Dec 26, 2020
 *      Author: fil
 */


#include "main.h"
#include "math.h"
#include "../audio_component.h"

__attribute__ ((aligned (16))) OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];

uint32_t FindOscillatorByMidiNote(uint8_t midi_note)
{
uint32_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
		if ( Oscillator[osc_number].midi_note == midi_note )
			return osc_number;
	return NUMOSCILLATORS+1;
}


uint32_t FindFreeOscillator(void)
{
uint32_t		osc_number;
uint8_t			osc_ret = 0;
uint32_t		oldest_osc=0;

	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number+=VOICES)
	{
		if ( Oscillator[osc_number].state == OSC_OFF )
		{
			return osc_number;
		}
		else
		{
			if ( Oscillator[osc_number].oscillator_age > oldest_osc )
			{
				oldest_osc = Oscillator[osc_number].oscillator_age;
				osc_ret = osc_number;
			}
		}
	}
	oldest_osc=0;
	return osc_ret;
}

void DisableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity)
{
uint8_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		if (( Oscillator[osc_number].midi_note == midi_note ) && ( Oscillator[osc_number].state == OSC_ON ))
		{
			Oscillator[osc_number].state = OSC_GO_OFF;
		}
	}
}

void DisableAllOscillator(void)
{
uint8_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		Oscillator[osc_number].state = OSC_GO_OFF;
	}
}

void EnableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity)
{
float	delta_phase;
float	freq;
uint32_t	osc_number,i;

	osc_number = FindFreeOscillator();
	for(i=0 ; i< VOICES; i++)
	{
		freq = midi_freq[midi_note] + SystemFlags.fosc_detune[i]  + SystemFlags.tuner_delta_multiplier;
		delta_phase = (float )WAVETABLE_SIZE / ((float )SystemParameters.audio_sampling_frequency / freq);
		Oscillator[osc_number+i].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
		Oscillator[osc_number+i].current_phase = 0;
		Oscillator[osc_number+i].midi_note = midi_note;
		Oscillator[osc_number+i].state = OSC_ON;
		Oscillator[osc_number+i].oscillator_age = 0;
		Oscillator[osc_number+i].adsr_state = OSC_A_STATE;
		Oscillator[osc_number+i].volume = ((float )SystemFlags.osc_volume[i] / 10.0F);

		SetADSR_oscParams(osc_number+i,velocity);

		switch(SystemFlags.osc_waves[i])
		{
		case	TRIANGLE 	: Oscillator[osc_number+i].waveform = TRIANGLE; break;
		case	SQUARE 		: Oscillator[osc_number+i].waveform = SQUARE; break;
		case	NOISE 		: Oscillator[osc_number+i].waveform = NOISE; break;
		default 			: Oscillator[osc_number+i].waveform = SINE; break;
		}
	}
}

void InitOscillators(void)
{
float	delta_phase;
float	freq;
uint16_t	osc_number;

	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		SetADSR_oscParams(osc_number,0);
		Oscillator[osc_number].current_phase = 0.0f;
		Oscillator[osc_number].detune = 0.0f;
		Oscillator[osc_number].state = OSC_OFF;
		Oscillator[osc_number].waveform = SINE;
		Oscillator[osc_number].midi_note = INVALID_MIDI_NOTE;
		Oscillator[osc_number].duty = 50*DUTY_SCALE;
		Oscillator[osc_number].volume = 1.0F;
		freq = midi_freq[69] + Oscillator[osc_number].detune;
		delta_phase = (float )WAVETABLE_SIZE / ((float )SystemParameters.audio_sampling_frequency / freq);
		Oscillator[osc_number].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
	}
	InitOscillatorsTables();
}
