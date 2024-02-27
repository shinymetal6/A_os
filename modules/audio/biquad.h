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
 * biquad.h
 *
 *  Created on: Feb 27, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_BIQUAD_H_
#define MODULES_AUDIO_BIQUAD_H_

typedef struct _BIQUAD_TypeDef
{
	float a0, a1, a2, a3, a4;
	float x1, x2, y1, y2;
	float resonance;
} BIQUAD_TypeDef;

#define	BIQUAD_HIGH_PASS	0
#define	BIQUAD_LOW_PASS		1
#define	BIQUAD_BAND_PASS	2
#define	BIQUAD_NOTCH		3
#define	BIQUAD_PEQ			4
#define	BIQUAD_LSH			5
#define	BIQUAD_HSH			6


extern int16_t Do_Biquad_s(int16_t inputData);
extern	void Biquad_init(uint8_t Type, uint16_t Frequency, float iir_Q_BW, float resonance);
extern	void Biquad_enable(void);
extern	void Biquad_disable(void);
extern	void Biquad_configure(int type, float Frequency, float iir_Q_BW, float resonance);

#endif /* MODULES_AUDIO_BIQUAD_H_ */
