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
#include "../../kernel/system_default.h"	/* for BOARD_NAME variable only */

#ifdef SYNTH_ENGINE_ENABLE
#include "oscillator_core.h"

extern OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];

extern	SystemFlagsTypeDef	SystemFlags;

uint16_t osc_sine_tab[AUDIO_BUF_SIZE] =
{
16384, 16786, 17187, 17589, 17989, 18389, 18787, 19184,
19580, 19973, 20364, 20753, 21139, 21523, 21903, 22280,
22653, 23023, 23388, 23750, 24107, 24459, 24806, 25149,
25486, 25817, 26143, 26463, 26777, 27085, 27386, 27681,
27968, 28249, 28523, 28789, 29048, 29299, 29543, 29778,
30006, 30225, 30436, 30639, 30832, 31018, 31194, 31361,
31520, 31669, 31809, 31940, 32062, 32174, 32276, 32369,
32452, 32526, 32590, 32644, 32688, 32723, 32747, 32762,
32767, 32762, 32747, 32723, 32688, 32644, 32590, 32526,
32452, 32369, 32276, 32174, 32062, 31940, 31809, 31669,
31520, 31361, 31194, 31018, 30832, 30639, 30436, 30225,
30006, 29778, 29543, 29299, 29048, 28789, 28523, 28249,
27968, 27681, 27386, 27085, 26777, 26463, 26143, 25817,
25486, 25149, 24806, 24459, 24107, 23750, 23388, 23023,
22653, 22280, 21903, 21523, 21139, 20753, 20364, 19973,
19580, 19184, 18787, 18389, 17989, 17589, 17187, 16786,
16384, 15981, 15580, 15178, 14778, 14378, 13980, 13583,
13187, 12794, 12403, 12014, 11628, 11244, 10864, 10487,
10114, 9744, 9379, 9017, 8660, 8308, 7961, 7618,
7281, 6950, 6624, 6304, 5990, 5682, 5381, 5086,
4799, 4518, 4244, 3978, 3719, 3468, 3224, 2989,
2761, 2542, 2331, 2128, 1935, 1749, 1573, 1406,
1247, 1098, 958, 827, 705, 593, 491, 398,
315, 241, 177, 123, 79, 44, 20, 5,
0, 5, 20, 44, 79, 123, 177, 241,
315, 398, 491, 593, 705, 827, 958, 1098,
1247, 1406, 1573, 1749, 1935, 2128, 2331, 2542,
2761, 2989, 3224, 3468, 3719, 3978, 4244, 4518,
4799, 5086, 5381, 5682, 5990, 6304, 6624, 6950,
7281, 7618, 7961, 8308, 8660, 9017, 9379, 9744,
10114, 10487, 10864, 11244, 11628, 12014, 12403, 12794,
13187, 13583, 13980, 14378, 14778, 15178, 15580, 15981};

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
uint8_t	flag = 0;
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
	/*
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		oscout_buffer[i] = HALF_DAC_RESOLUTION + ((osc_buffer[i] >> 19) & (DAC_RESOLUTION-1));
		*/
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		oscout_buffer[i] = osc_buffer[i] >> 17;
	if ( Oscillator[osc_number].state != OSC_OFF )
		flag = 1;
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
