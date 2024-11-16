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
 * fft.h
 *
 *  Created on: Feb 24, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_FFT_H_
#define MODULES_AUDIO_FFT_H_

typedef struct _FftTypeDef
{
	uint8_t 	window_type;
} FftTypeDef;

#define	FFT_WINDOW_NONE		0
#define	FFT_WINDOW_HAMMING	1
#define	FFT_WINDOW_HANNING	2
#define	FFT_WINDOW_BLACKMAN	3

extern	float	float_fftmag_outbuf[HALF_NUMBER_OF_AUDIO_SAMPLES/2];

extern	void Fft_init(uint8_t WindowType);
extern	void Fft_enable(void);
extern	void Fft_disable(void);
extern	void Do_Fft(int16_t* inputData, int16_t* outputData);

#endif /* MODULES_AUDIO_FFT_H_ */
