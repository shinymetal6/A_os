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
#include "noise.h"

extern OscillatorsTypeDef	Oscillator[NUMOSCILLATORS];
extern	float	delta_k_phase;

#ifdef STM32H7xx_HAL_I2S_H
__attribute__((section(".table"))) __attribute__ ((aligned (32))) const int16_t rom_osc_sine_tab[WAVETABLE_SIZE] =
{
		0,402,803,1205,1605,2005,2403,2800,3196,3589,3980,4369,4755,5139,5519,5896,
		6269,6639,7004,7365,7722,8075,8422,8764,9101,9433,9759,10079,10393,10700,11002,11296,
		11584,11865,12139,12405,12664,12915,13158,13394,13621,13841,14052,14254,14448,14633,14810,14977,
		15135,15285,15425,15556,15677,15789,15892,15984,16068,16141,16205,16259,16304,16338,16363,16378,
		16383,16378,16363,16338,16304,16259,16205,16141,16068,15984,15892,15789,15677,15556,15425,15285,
		15135,14977,14810,14633,14448,14254,14052,13841,13621,13394,13158,12915,12664,12405,12139,11865,
		11584,11296,11002,10700,10393,10079,9759,9433,9101,8764,8422,8075,7722,7365,7004,6639,
		6269,5896,5519,5139,4755,4369,3980,3589,3196,2800,2403,2005,1605,1205,803,402,
		0,-402,-803,-1205,-1605,-2005,-2403,-2800,-3196,-3589,-3980,-4369,-4755,-5139,-5519,-5896,
		-6269,-6639,-7004,-7365,-7722,-8075,-8422,-8764,-9101,-9433,-9759,-10079,-10393,-10700,-11002,-11296,
		-11584,-11865,-12139,-12405,-12664,-12915,-13158,-13394,-13621,-13841,-14052,-14254,-14448,-14633,-14810,-14977,
		-15135,-15285,-15425,-15556,-15677,-15789,-15892,-15984,-16068,-16141,-16205,-16259,-16304,-16338,-16363,-16378,
		-16383,-16378,-16363,-16338,-16304,-16259,-16205,-16141,-16068,-15984,-15892,-15789,-15677,-15556,-15425,-15285,
		-15135,-14977,-14810,-14633,-14448,-14254,-14052,-13841,-13621,-13394,-13158,-12915,-12664,-12405,-12139,-11865,
		-11584,-11296,-11002,-10700,-10393,-10079,-9759,-9433,-9101,-8764,-8422,-8075,-7722,-7365,-7004,-6639,
		-6269,-5896,-5519,-5139,-4755,-4369,-3980,-3589,-3196,-2800,-2403,-2005,-1605,-1205,-803,-402,
};
#endif
#ifdef STM32H7xx_HAL_DAC_H
__attribute__((section(".table"))) __attribute__ ((aligned (32))) const uint16_t rom_osc_sine_tab[WAVETABLE_SIZE] =
{
		0x800, 0x832, 0x864, 0x896, 0x8c8, 0x8fa, 0x92c, 0x95e, 0x98f, 0x9c0, 0x9f1, 0xa22, 0xa52, 0xa82, 0xab1, 0xae0,
		0xb0f, 0xb3d, 0xb6b, 0xb98, 0xbc5, 0xbf1, 0xc1c, 0xc47, 0xc71, 0xc9a, 0xcc3, 0xceb, 0xd12, 0xd39, 0xd5f, 0xd83,
		0xda7, 0xdca, 0xded, 0xe0e, 0xe2e, 0xe4e, 0xe6c, 0xe8a, 0xea6, 0xec1, 0xedc, 0xef5, 0xf0d, 0xf24, 0xf3a, 0xf4f,
		0xf63, 0xf76, 0xf87, 0xf98, 0xfa7, 0xfb5, 0xfc2, 0xfcd, 0xfd8, 0xfe1, 0xfe9, 0xff0, 0xff5, 0xff9, 0xffd, 0xffe,
		0xfff, 0xffe, 0xffd, 0xff9, 0xff5, 0xff0, 0xfe9, 0xfe1, 0xfd8, 0xfcd, 0xfc2, 0xfb5, 0xfa7, 0xf98, 0xf87, 0xf76,
		0xf63, 0xf4f, 0xf3a, 0xf24, 0xf0d, 0xef5, 0xedc, 0xec1, 0xea6, 0xe8a, 0xe6c, 0xe4e, 0xe2e, 0xe0e, 0xded, 0xdca,
		0xda7, 0xd83, 0xd5f, 0xd39, 0xd12, 0xceb, 0xcc3, 0xc9a, 0xc71, 0xc47, 0xc1c, 0xbf1, 0xbc5, 0xb98, 0xb6b, 0xb3d,
		0xb0f, 0xae0, 0xab1, 0xa82, 0xa52, 0xa22, 0x9f1, 0x9c0, 0x98f, 0x95e, 0x92c, 0x8fa, 0x8c8, 0x896, 0x864, 0x832,
		0x800, 0x7cd, 0x79b, 0x769, 0x737, 0x705, 0x6d3, 0x6a1, 0x670, 0x63f, 0x60e, 0x5dd, 0x5ad, 0x57d, 0x54e, 0x51f,
		0x4f0, 0x4c2, 0x494, 0x467, 0x43a, 0x40e, 0x3e3, 0x3b8, 0x38e, 0x365, 0x33c, 0x314, 0x2ed, 0x2c6, 0x2a0, 0x27c,
		0x258, 0x235, 0x212, 0x1f1, 0x1d1, 0x1b1, 0x193, 0x175, 0x159, 0x13e, 0x123, 0x10a, 0xf2, 0xdb, 0xc5, 0xb0,
		0x9c, 0x89, 0x78, 0x67, 0x58, 0x4a, 0x3d, 0x32, 0x27, 0x1e, 0x16, 0xf, 0xa, 0x6, 0x2, 0x1,
		0x0, 0x1, 0x2, 0x6, 0xa, 0xf, 0x16, 0x1e, 0x27, 0x32, 0x3d, 0x4a, 0x58, 0x67, 0x78, 0x89,
		0x9c, 0xb0, 0xc5, 0xdb, 0xf2, 0x10a, 0x123, 0x13e, 0x159, 0x175, 0x193, 0x1b1, 0x1d1, 0x1f1, 0x212, 0x235,
		0x258, 0x27c, 0x2a0, 0x2c6, 0x2ed, 0x314, 0x33c, 0x365, 0x38e, 0x3b8, 0x3e3, 0x40e, 0x43a, 0x467, 0x494, 0x4c2,
		0x4f0, 0x51f, 0x54e, 0x57d, 0x5ad, 0x5dd, 0x60e, 0x63f, 0x670, 0x6a1, 0x6d3, 0x705, 0x737, 0x769, 0x79b, 0x7cd,
};
#endif

AUDIO_FAST_RAM	int32_t		osc_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
AUDIO_FAST_RAM	int16_t		oscout_buffer[HALF_NUMBER_OF_AUDIO_SAMPLES];
AUDIO_FAST_RAM	int32_t		osc_buffer_gen[HALF_NUMBER_OF_AUDIO_SAMPLES];
AUDIO_FAST_RAM	int16_t 	osc_sine_tab[WAVETABLE_SIZE];
AUDIO_FAST_RAM	int16_t		osc_square_tab[WAVETABLE_SIZE];
AUDIO_FAST_RAM	int16_t		osc_tri_tab[WAVETABLE_SIZE];
AUDIO_FAST_RAM	int16_t		osc_noise_tab[WAVETABLE_SIZE];

__attribute__((section(".table"))) __attribute__ ((aligned (32))) const float	rom_midi_freq[MIDI_NOTES] =
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
float	midi_freq[MIDI_NOTES];

ITCM_AREA_CODE	void RunOscillator32(void)
{
uint16_t	i;
uint8_t		angle,osc_number;

	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		osc_buffer[i] = 0;
	for(osc_number=0;osc_number<NUMOSCILLATORS;osc_number++)
	{
		if ((Oscillator[osc_number].state & OSCILLATOR_ON ) == OSCILLATOR_ON )
		{
			Oscillator[osc_number].oscillator_age++;
			for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
			{
				angle = (uint8_t )(Oscillator[osc_number].current_phase >> 8);

				osc_buffer_gen[i] = Oscillator[osc_number].wave[angle];
				Oscillator[osc_number].current_phase += Oscillator[osc_number].delta_phase;
				// zero crossing
				if ((Oscillator[osc_number].state & OSCILLATOR_GO_OFF ) == OSCILLATOR_GO_OFF )
				{
					if ( ( i > 1) && (i < WAVETABLE_SIZE-1 ))
					{
						if ( (osc_buffer_gen[i-1] < 0 ) || (osc_buffer_gen[i] > 0) )
							Oscillator[osc_number].state = ~( OSCILLATOR_ON | OSCILLATOR_GO_OFF);
					}
				}
				if ((Oscillator[osc_number].state & OSCILLATOR_ON ) != OSCILLATOR_ON )
					osc_buffer_gen[i] = 0;
				// zero crossing end
				osc_buffer[i] += (int32_t )( (float )osc_buffer_gen[i] * Oscillator[osc_number].volume);
			}
		}
	}
	for ( i=0;i<HALF_NUMBER_OF_AUDIO_SAMPLES;i++)
		oscout_buffer[i] = osc_buffer[i] >> (VOICES / 2);
}

ITCM_AREA_CODE	void ChangeOscillatorWavePhase(uint16_t	osc_number ,uint8_t waveform )
{
	switch (waveform)
	{
	case	SINE:
		Oscillator[osc_number & 0x03].wave = osc_sine_tab;
		break;
	case	TRIANGLE:
		Oscillator[osc_number & 0x03].wave = osc_tri_tab;
		break;
	case	SQUARE:
		Oscillator[osc_number & 0x03].wave = osc_square_tab;
		break;
	case	NOISE:
		Oscillator[osc_number & 0x03].wave = osc_noise_tab;
		break;
	default :
		break;
	}
}

void InitOscillatorsTables(void)
{
uint16_t	i,tri_up = 1;
int16_t		tri_delta = MIN_SINEVAL;

	// 1 setup midi table conversion
	for(i=0;i<MIDI_NOTES;i++)
		midi_freq[i] = rom_midi_freq[i];
	// 2 setup tables in DTCMRAM
	for(i=0;i<NUMBER_OF_AUDIO_SAMPLES;i++)
	{
		// 2.1 sine
		osc_sine_tab[i] = rom_osc_sine_tab[i];
		// 2.2 square
		osc_square_tab[i] = MAX_SINEVAL;
		if ( i < AUDIO_BUF_SIZE/2 )
			osc_square_tab[i] = MIN_SINEVAL;
		// 2.3 triangle
		osc_tri_tab[i] = tri_delta;
		if ( tri_up == 1 )
		{
			tri_delta += MAX_SINEVAL /  (WAVETABLE_SIZE/4);
			if ( tri_delta >= MAX_SINEVAL)
				tri_up = 0;
		}
		else
			tri_delta -= MAX_SINEVAL /  (WAVETABLE_SIZE/4);
	}
	// 2.4 create noise table
	//Noise_Gen(osc_noise_tab);
	// 3 setup default table -> sine
	for(i=0;i<NUMOSCILLATORS;i++)
	{
		Oscillator[i].wave = osc_sine_tab;
	}
}

#endif // #ifdef AUDIO_GENERATORS_ENABLED

