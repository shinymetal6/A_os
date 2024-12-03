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
#include "../../../kernel/kernel_opt.h"

#include "../audio.h"
#include "../effects.h"

#ifdef AUDIO_GENERATORS_ENABLED

#include "oscillators.h"
#include "oscillator_core.h"

AUDIO_FAST_RAM					OscillatorsFlagsTypeDef			OscillatorsFlags;
extern	AudioFlagsTypeDef		AudioFlags;

AUDIO_FAST_RAM	__attribute__ ((aligned (16))) OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];

ITCM_AREA_CODE	uint32_t FindOscillatorByMidiNote(uint8_t midi_note)
{
uint32_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
		if ( Oscillator[osc_number].midi_note == midi_note )
			return osc_number;
	return NUMOSCILLATORS+1;
}

ITCM_AREA_CODE	uint32_t FindFreeOscillator(void)
{
uint32_t		osc_number;
uint8_t			osc_ret = 0;
uint32_t		oldest_osc=0;

	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number+=VOICES)
	{
		if ( (Oscillator[osc_number].state & OSCILLATOR_ON ) != OSCILLATOR_ON )
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

ITCM_AREA_CODE	void DisableOscillator( uint16_t midi_note , uint8_t velocity)
{
uint8_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		if (( Oscillator[osc_number].midi_note == midi_note ) && ((Oscillator[osc_number].state & OSCILLATOR_ON ) == OSCILLATOR_ON ))
		{
			Oscillator[osc_number].state |= OSCILLATOR_GO_OFF;
			//Oscillator[osc_number].state &= ~OSCILLATOR_ON;
		}
	}
}

ITCM_AREA_CODE	void DisableAllOscillator(void)
{
uint8_t	osc_number;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		//Oscillator[osc_number].state |= OSCILLATOR_GO_OFF;
		Oscillator[osc_number].state &= ~OSCILLATOR_ON;
	}
}

extern	float	midi_freq[128];
float delta_phase_k =  (float )WAVETABLE_SIZE / (float )SAMPLE_FREQUENCY;

ITCM_AREA_CODE	void EnableOscillator(uint16_t midi_note , uint8_t velocity)
{
float	delta_phase;
float	freq;
uint32_t	osc_number,i;

	osc_number = FindFreeOscillator();
	for(i=0 ; i< VOICES; i++)
	{
		Oscillator[osc_number].midi_note = midi_note;
		freq = midi_freq[Oscillator[osc_number].midi_note] + Oscillator[osc_number].detune;
		delta_phase = (float )WAVETABLE_SIZE / ((float )(SAMPLE_FREQUENCY*(AUDIO_BUF_SIZE / 256)) / freq);
		Oscillator[osc_number+i].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);

		Oscillator[osc_number+i].current_phase = 0;
		Oscillator[osc_number+i].midi_note = midi_note;
		Oscillator[osc_number+i].velocity = velocity;
		Oscillator[osc_number+i].state = OSCILLATOR_ON;
		Oscillator[osc_number+i].oscillator_age = 0;
		Oscillator[osc_number+i].volume = ((float )OscillatorsFlags.osc_volume[i] / 10.0F);
		/* TEST */
		Oscillator[osc_number+i].volume = ((float )0.5F);
		/* TEST END */
	}
}

void InitOscillators(void)
{
float	freq,delta_phase;
uint16_t	osc_number;

	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		Oscillator[osc_number].current_phase = 0.0f;
		Oscillator[osc_number].detune = 0.0f;
		Oscillator[osc_number].state &= ~OSCILLATOR_ON;
		Oscillator[osc_number].waveform = SINE;
		Oscillator[osc_number].midi_note = INVALID_MIDI_NOTE;
		Oscillator[osc_number].duty = 50*DUTY_SCALE;
		Oscillator[osc_number].volume = 1.0F;
		freq = midi_freq[69] + Oscillator[osc_number].detune;
		delta_phase = (float )WAVETABLE_SIZE / ((float )SAMPLE_FREQUENCY / freq);
		Oscillator[osc_number].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
		Oscillator[osc_number].delta_phase = 0;
		Oscillator[osc_number].state = 0;
	}
	InitOscillatorsTables();
}

#endif // #ifdef AUDIO_GENERATORS_ENABLED

