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
 * oscillator_core.c
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE
#include "oscillator_core.h"

extern OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];

extern	SystemFlagsTypeDef	SystemFlags;

int16_t osc_sine_tab[AUDIO_BUF_SIZE] =
{
	0x0,0x5,0x14,0x2c,0x4f,0x7b,0xb1,0xf1,
	0x13b,0x18e,0x1eb,0x251,0x2c1,0x33b,0x3be,0x44a,
	0x4df,0x57e,0x625,0x6d5,0x78f,0x850,0x91b,0x9ee,
	0xac9,0xbad,0xc98,0xd8c,0xe87,0xf8a,0x1094,0x11a6,
	0x12bf,0x13de,0x1505,0x1632,0x1766,0x18a0,0x19e0,0x1b26,
	0x1c71,0x1dc2,0x1f19,0x2074,0x21d4,0x2339,0x24a3,0x2610,
	0x2782,0x28f7,0x2a70,0x2bec,0x2d6c,0x2eee,0x3073,0x31fa,
	0x3383,0x350f,0x369c,0x382a,0x39ba,0x3b4a,0x3cdc,0x3e6d,
	0x4000,0x4192,0x4323,0x44b5,0x4645,0x47d5,0x4963,0x4af0,
	0x4c7c,0x4e05,0x4f8c,0x5111,0x5293,0x5413,0x558f,0x5708,
	0x587d,0x59ef,0x5b5c,0x5cc6,0x5e2b,0x5f8b,0x60e6,0x623d,
	0x638e,0x64d9,0x661f,0x675f,0x6899,0x69cd,0x6afa,0x6c21,
	0x6d40,0x6e59,0x6f6b,0x7075,0x7178,0x7273,0x7367,0x7452,
	0x7536,0x7611,0x76e4,0x77af,0x7870,0x792a,0x79da,0x7a81,
	0x7b20,0x7bb5,0x7c41,0x7cc4,0x7d3e,0x7dae,0x7e14,0x7e71,
	0x7ec4,0x7f0e,0x7f4e,0x7f84,0x7fb0,0x7fd3,0x7feb,0x7ffa,
	0x7fff,0x7ffa,0x7feb,0x7fd3,0x7fb0,0x7f84,0x7f4e,0x7f0e,
	0x7ec4,0x7e71,0x7e14,0x7dae,0x7d3e,0x7cc4,0x7c41,0x7bb5,
	0x7b20,0x7a81,0x79da,0x792a,0x7870,0x77af,0x76e4,0x7611,
	0x7536,0x7452,0x7367,0x7273,0x7178,0x7075,0x6f6b,0x6e59,
	0x6d40,0x6c21,0x6afa,0x69cd,0x6899,0x675f,0x661f,0x64d9,
	0x638e,0x623d,0x60e6,0x5f8b,0x5e2b,0x5cc6,0x5b5c,0x59ef,
	0x587d,0x5708,0x558f,0x5413,0x5293,0x5111,0x4f8c,0x4e05,
	0x4c7c,0x4af0,0x4963,0x47d5,0x4645,0x44b5,0x4323,0x4192,
	0x4000,0x3e6d,0x3cdc,0x3b4a,0x39ba,0x382a,0x369c,0x350f,
	0x3383,0x31fa,0x3073,0x2eee,0x2d6c,0x2bec,0x2a70,0x28f7,
	0x2782,0x2610,0x24a3,0x2339,0x21d4,0x2074,0x1f19,0x1dc2,
	0x1c71,0x1b26,0x19e0,0x18a0,0x1766,0x1632,0x1505,0x13de,
	0x12bf,0x11a6,0x1094,0xf8a,0xe87,0xd8c,0xc98,0xbad,
	0xac9,0x9ee,0x91b,0x850,0x78f,0x6d5,0x625,0x57e,
	0x4df,0x44a,0x3be,0x33b,0x2c1,0x251,0x1eb,0x18e,
	0x13b,0xf1,0xb1,0x7b,0x4f,0x2c,0x14,0x5,
};

__attribute__((section(".table"))) const float	midi_freq[128] =
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
		13.750, /* # 1 */
		14.568,
		15.434,
		16.352,
		17.324,
		18.354,
		19.445,
		20.601,
		21.826,
		23.124,
		24.499,
		25.956,
		27.500, /* # 2 */
		29.135,
		30.867,
		32.703,
		34.648,
		36.708,
		38.890,
		41.203,
		43.653,
		46.249,
		48.999,
		51.913,
		55.000, /* # 3 */
		58.270,
		61.735,
		65.406,
		69.295,
		73.416,
		77.781,
		82.406,
		87.307,
		92.499,
		97.998,
		103.82,
		110.00, /* # 4 */
		116.54,
		123.47,
		130.81,
		138.59,
		146.83,
		155.56,
		164.81,
		174.61,
		184.99,
		195.99,
		207.65,
		220.00, /* # 5 */
		233.08,
		246.94,
		261.63,
		277.18,
		293.66,
		311.13,
		329.63,
		349.23,
		369.99,
		391.99,
		415.31,
		440.00, /* # 6 */
		466.16,
		489.88,
		523.25,
		554.37,
		587.33,
		622.25,
		659.26,
		698.46,
		739.99,
		783.99,
		830.61,
		880.00, /* # 7 */
		932.32,
		987.77,
		1046.5,
		1108.7,
		1174.7,
		1244.5,
		1318.5,
		1396.9,
		1480.0,
		1568.0,
		1661.2,
		1760.0, /* # 8 */
		1864.7,
		1975.5,
		2093.0,
		2217.5,
		2349.3,
		2489.0,
		2637.0,
		2793.8,
		2960.0,
		3136.0,
		3322.4,
		3520.0, /* # 9 */
		3729.3,
		3951.1,
		4186.0,
		4434.9,
		4698.6,
		4978.0,
		5274.0,
		5587.7,
		5919.9,
		6271.9,
		6644.9,
		7040.0, /* # 10 */
		7458.6,
		7902.1,
		8372.0,
		8869.8,
		9397.3,
		9956.1,
		10548.1,
		11175.3,
		11839.8,
		12543.9
};

/*
__attribute__ ((aligned (4)))	uint16_t	pipe0[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (4)))	uint16_t	pipe1[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (4)))	uint16_t	pipe2[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (16)))	uint32_t	osc_buffer_gen[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (16)))	uint16_t	noise_buffer_gen[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (16)))	uint32_t	osc_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (16)))	uint16_t	oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
*/
__attribute__ ((aligned (16)))	uint32_t	osc_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (16)))	uint16_t	oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (16)))	uint32_t	osc_buffer_gen[HALF_NUMBER_OF_AUDIO_SAMPLES];
__attribute__ ((aligned (16)))	uint16_t	noise_buffer_gen[HALF_NUMBER_OF_AUDIO_SAMPLES];

uint16_t        tritab[VOICES][WAVETABLE_SIZE];

#define	STEP_UNIT	(float )((SAMPLE_FREQUENCY) / 1000.0F)
float	stunit;

void SetADSR_oscParams(uint32_t osc_number, uint8_t velocity )
{
	float	Atime,Dtime,Rtime;
	Atime = 100.0F;
	Dtime = 100.0F;
	Rtime = 200.0F;
	stunit = Atime * (float )((SAMPLE_FREQUENCY) / 1000.0F);
	Oscillator[osc_number].Alevel = 0;

	Oscillator[osc_number].Avalue = (float )(DAC_RESOLUTION-1);
	Oscillator[osc_number].Dvalue = (float )((DAC_RESOLUTION-1)*0.7F);

	Oscillator[osc_number].adsr_state = OSC_A_STATE;

	Oscillator[osc_number].Astep = Oscillator[osc_number].Avalue / (Atime * (float )(STEP_UNIT));
	Oscillator[osc_number].Dstep = Oscillator[osc_number].Dvalue / (Dtime * (float )(STEP_UNIT));
	Oscillator[osc_number].Rstep = Oscillator[osc_number].Dvalue / (Rtime * (float )(STEP_UNIT));

	if ( Oscillator[osc_number].Astep == 0.0F )
		Oscillator[osc_number].Astep = 1.0F;
	if ( Oscillator[osc_number].Dstep == 0.0F )
		Oscillator[osc_number].Dstep = 1.0F;
	if ( Oscillator[osc_number].Rstep == 0.0F )
		Oscillator[osc_number].Rstep = 1.0F;
}

static uint16_t f_adsr(uint32_t osc_number, uint8_t	angle )
{
float ret_val = 0.0F;	//Oscillator[osc_number].midi_note

	if ( Oscillator[osc_number].state != OSC_OFF )
	{
		Oscillator[osc_number].oscillator_age++;
		{
			switch(Oscillator[osc_number].adsr_state)
			{
			case	OSC_A_STATE:
				Oscillator[osc_number].Alevel +=  Oscillator[osc_number].Astep;
				if ( Oscillator[osc_number].Alevel >= Oscillator[osc_number].Avalue )
				{
					Oscillator[osc_number].Dlevel = Oscillator[osc_number].Alevel;
					Oscillator[osc_number].adsr_state = OSC_D_STATE;
				}
				ret_val = Oscillator[osc_number].Alevel;
				break;
			case	OSC_D_STATE:
				Oscillator[osc_number].Dlevel -=  Oscillator[osc_number].Dstep;
				if ( Oscillator[osc_number].Dlevel <= Oscillator[osc_number].Dvalue )
				{
					Oscillator[osc_number].Slevel = Oscillator[osc_number].Dlevel;
					Oscillator[osc_number].adsr_state = OSC_S_STATE;
				}
				ret_val = Oscillator[osc_number].Dlevel;
				break;
			case	OSC_S_STATE:
				if ( Oscillator[osc_number].state == OSC_GO_OFF)
				{
					Oscillator[osc_number].adsr_state = OSC_R_STATE;
					Oscillator[osc_number].Rlevel = Oscillator[osc_number].Slevel;
				}
				ret_val = Oscillator[osc_number].Slevel;
				break;
			case	OSC_R_STATE:
				Oscillator[osc_number].Rlevel -=  Oscillator[osc_number].Rstep;
				if ( Oscillator[osc_number].Rlevel <= Oscillator[osc_number].Rstep )
					Oscillator[osc_number].adsr_state = ADSR_CLOSE_STATE;
				ret_val = Oscillator[osc_number].Rlevel;
				break;
			case	ADSR_CLOSE_STATE:
				if ( angle > 224 )
				{
					Oscillator[osc_number].state = OSC_OFF;
					//Oscillator[osc_number].current_volume = 0;
					Oscillator[osc_number].midi_note = INVALID_MIDI_NOTE;
					Oscillator[osc_number].oscillator_age = 0;
					Oscillator[osc_number].Alevel = 0.0F;
					ret_val = 0.0F;
				}
				break;
			}
		}
	}
	return (uint16_t )ret_val;
}

void RunOscillator32(void)
{
uint16_t	i;
uint8_t		angle,osc_number;
uint16_t	adsr_value;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		osc_buffer[i] = 0;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		if ( Oscillator[osc_number].state != OSC_OFF )
		{
			for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			{
				angle = (uint8_t )(Oscillator[osc_number].current_phase >> 8);
				adsr_value = f_adsr(osc_number,angle );
				switch(Oscillator[osc_number].waveform)
				{
				case	TRIANGLE	:	osc_buffer_gen[i] = (tritab[osc_number & 0x03][angle] * adsr_value ); break;
				case	SQUARE		:
					if ( angle >  (uint8_t )((float )Oscillator[osc_number & 0x03].duty) )
						osc_buffer_gen[i] = ((DAC_RESOLUTION-1) * adsr_value );
					else
						osc_buffer_gen[i] = 0;
					break;
				case NOISE			:	osc_buffer_gen[i] = noise_buffer_gen[i] << 16 | noise_buffer_gen[(i+1) & (HALF_NUMBER_OF_AUDIO_SAMPLES-1)];break;
				default				:	osc_buffer_gen[i] = (osc_sine_tab[angle] * adsr_value ); break;
				}
				osc_buffer[i] += ((float )osc_buffer_gen[i] * Oscillator[osc_number].volume);

				Oscillator[osc_number].current_phase += Oscillator[osc_number].delta_phase;

				if ( Oscillator[osc_number].midi_note != INVALID_MIDI_NOTE)
				{
					if (( SystemFlags.oscillator_flags & OSC_TUNE_PENDING ) == OSC_TUNE_PENDING)
					{
						float	delta_phase;
						float	freq;

						freq = midi_freq[Oscillator[osc_number].midi_note] + Oscillator[osc_number].detune + SystemFlags.tuner_delta_multiplier;
						delta_phase = (float )WAVETABLE_SIZE / ((float )SAMPLE_FREQUENCY / freq);
						Oscillator[osc_number].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
					}
				}
			}
		}
	}
	SystemFlags.oscillator_flags &= ~OSC_TUNE_PENDING;
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		oscout_buffer[i] = HALF_DAC_RESOLUTION + ((osc_buffer[i] >> 19) & (DAC_RESOLUTION-1));
}
/*
uint8_t	vco_cv = 0;

void RunOscillator4(void)
{
float	delta_phase;
uint16_t	i;
uint8_t		angle,osc_number;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		osc_buffer[i] = 0;
		for(osc_number=0;osc_number<VOICES;osc_number++)
		{
			angle = (uint8_t )(Oscillator[osc_number].current_phase >> 8);
			switch(Oscillator[osc_number].waveform)
			{
			case	SINE	:	osc_buffer[i] += (uint16_t )((float )osc_sine_tab[angle] * Oscillator[osc_number].volume);break;
			case 	TRIANGLE:	osc_buffer[i] += (uint16_t )((float )tritab[osc_number & 0x03][angle] * Oscillator[osc_number].volume);break;
			case 	SQUARE	:
				if ( angle >  (uint8_t )((float )Oscillator[osc_number & 0x03].duty * (100.0F/256.0F)) )
					osc_buffer[i] += (uint16_t )((float )(DAC_RESOLUTION-1) * Oscillator[osc_number].volume);
				break;
			default			:	osc_buffer[i] += (uint16_t )((float )osc_sine_tab[angle] * Oscillator[osc_number].volume);break;	// SINE is default
			}
			Oscillator[osc_number].current_phase += Oscillator[osc_number].delta_phase;
			if ( vco_cv != 0 )
			{
				delta_phase = (float )WAVETABLE_SIZE / ((float )SAMPLE_FREQUENCY / ( SystemFlags.oscillator_tuner_constant + Oscillator[osc_number].detune));
				Oscillator[osc_number].delta_phase = (uint16_t )(delta_phase * (float )INT_PRECISION);
			}
			else
				Oscillator[osc_number].current_phase = 0;
		}
		oscout_buffer[i] = osc_buffer[i] >> 2;
	}
	return;
}
*/
static void triCompile(uint16_t	osc_number , uint32_t phase)
{
float 	step_up,step_down,current;
uint16_t	i , k;

	k = osc_number;
	k &= (VOICES-1);

	step_up = (float )(DAC_RESOLUTION-1) / (float )phase;
	step_down = (float )(DAC_RESOLUTION-1)  / (float )( WAVETABLE_SIZE - phase);
	current=0;
	for(i=0;i<WAVETABLE_SIZE;i++)
	{
		if ( i < phase )
		{
			tritab[k][i] = (uint32_t )current;
			current += step_up;
		}
		else
		{
			tritab[k][i] = (uint32_t )current;
			current -= step_down;
		}
	}
}

void ChangeOscillatorWavePhase(uint16_t	osc_number ,uint8_t waveform , uint16_t phase )
{
	switch (waveform)
	{
	case	TRIANGLE:	triCompile(osc_number,phase);break;
	default : break;
	}
	SystemFlags.osc_duty[osc_number & 0x03] = phase;
}

void InitOscillatorsTables(void)
{
uint16_t	i;
	for(i=0;i<VOICES;i++)
	{
		//squareCompile(i , 127);
		triCompile(i , 127);
	}
}

#endif
