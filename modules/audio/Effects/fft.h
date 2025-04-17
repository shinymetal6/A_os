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
 *  Created on: Apr 16, 2025
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECTS_FFT_H_
#define MODULES_AUDIO_EFFECTS_FFT_H_

typedef struct
{
	uint8_t			status;
	uint8_t			initialized;
	uint8_t			flags;
}FFT_Effect_TypeDef;

#define	FFT_WINDOW_NONE		0
#define	FFT_WINDOW_BLACKMAN	1
#define	FFT_WINDOW_HAMMING	2
#define	FFT_WINDOW_HANNING	3

extern void Do_Fft(int16_t *inputData, int16_t *outputData, uint8_t index);

#endif /* MODULES_AUDIO_EFFECTS_FFT_H_ */
