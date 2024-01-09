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
 *  Created on: Jan 8, 2024
 *      Author: fil
 */


#include "main.h"
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE

#include "../../kernel/audio.h"
#include "oscillators.h"
#include "oscillator_core.h"
WaveLR_t WaveLR[AUDIO_BUF_SIZE];

__attribute__ ((aligned (16))) OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];
__attribute__ ((aligned (16))) SystemFlagsTypeDef	SystemFlags;

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

extern	float	midi_freq[128];

void EnableOscillator(uint16_t channel, uint16_t midi_note , uint8_t velocity)
{
float	delta_phase;
float	freq;
uint32_t	osc_number,i;

	osc_number = FindFreeOscillator();
	for(i=0 ; i< VOICES; i++)
	{
		freq = midi_freq[midi_note] + SystemFlags.fosc_detune[i]  + SystemFlags.tuner_delta_multiplier;
		delta_phase = (float )WAVETABLE_SIZE / ((float )SAMPLE_FREQUENCY / freq);
		Oscillator[osc_number+i].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
		Oscillator[osc_number+i].current_phase = 0;
		Oscillator[osc_number+i].midi_note = midi_note;
		Oscillator[osc_number+i].state = OSC_ON;
		Oscillator[osc_number+i].oscillator_age = 0;
		Oscillator[osc_number+i].adsr_state = OSC_A_STATE;
		Oscillator[osc_number+i].volume = ((float )SystemFlags.osc_volume[i] / 10.0F);
		/* TEST */
		Oscillator[osc_number+i].volume = ((float )1);
		/* TEST END */

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

uint32_t *InitOscillators(void)
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
		delta_phase = (float )WAVETABLE_SIZE / ((float )SAMPLE_FREQUENCY / freq);
		Oscillator[osc_number].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
	}
	InitOscillatorsTables();
	return (uint32_t *)WaveLR;
}

void get_limits(uint16_t *start,uint16_t *end)
{
	if (( SystemFlags.audio_flags & AUDIO_VCO_HALF_FLAG ) == AUDIO_VCO_HALF_FLAG)
	{
		*start=0;
		*end = HALF_NUMBER_OF_AUDIO_SAMPLES;
	}
	else
	{
		*start=HALF_NUMBER_OF_AUDIO_SAMPLES;
		*end = NUMBER_OF_AUDIO_SAMPLES;
	}
}
extern uint16_t	oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];

void IrqProcessSamples(void)
{
uint16_t	start,end,i;

	RunOscillator32();
	get_limits(&start,&end);
	for(i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		WaveLR[i+start].channel[AUDIO_LEFT_CH] = oscout_buffer[i];
		WaveLR[i+start].channel[AUDIO_RIGHT_CH] = oscout_buffer[i];
	}
	//Vca(&signal_out[start],oscout_buffer);


}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	SystemFlags.audio_flags |= (AUDIO_VCO_HALF_FLAG | AUDIO_READY_FLAG);
	IrqProcessSamples();
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	SystemFlags.audio_flags &= ~AUDIO_VCO_HALF_FLAG;
	SystemFlags.audio_flags |= AUDIO_READY_FLAG;
	IrqProcessSamples();
}


#endif
