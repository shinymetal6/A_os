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
 * moog_filters.h
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_MOOG1_FILTER_H_
#define MODULES_AUDIO_MOOG1_FILTER_H_

typedef struct _MoogParametersTypeDef
{
	/* Used for moog1 filter */
	float 	f, p, q;
	float 	b0, b1, b2, b3, b4;
	/* Used for moog2 filter */
	float 	out1,out2,out3,out4;
	float 	in1,in2,in3,in4;
	float	filterCutoff;
	float	filterResonance;
	float	filter2_constant01;
	uint8_t phaser_rate;
	uint8_t phaser_feedback;

}MoogParametersTypeDef;

extern	void Moog1_init(float Cutoff,float Resonance);
extern	void Moog1_enable(void);
extern	void Moog1_disable(void);
extern	void Do_Moog1(int16_t* inputData, int16_t* outputData);

extern	void Moog2_init(float Cutoff,float Resonance);
extern	void Moog2_enable(void);
extern	void Moog2_disable(void);
extern	void Do_Moog2(int16_t* inputData, int16_t* outputData);

#endif /* MODULES_AUDIO_MOOG1_FILTER_H_ */
