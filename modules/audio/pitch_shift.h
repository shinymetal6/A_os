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
 * pitch_shift.h
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_PITCH_SHIFT_H_
#define MODULES_AUDIO_PITCH_SHIFT_H_

typedef struct _PitchShiftTypeDef
{
	int 	WtrP;
	float 	Rd_P;
	float 	Shift;
	float 	CrossFade;
	float 	a0, a1, a2, b1, b2, hp_in_z1, hp_in_z2, hp_out_z1, hp_out_z2;

} PitchShiftTypeDef;

extern	void PitchShift_init(float Shift,float CrossFade);
extern	void PitchShift_enable(void);
extern	void PitchShift_disable(void);
extern	void Do_PitchShift(int16_t* inputData, int16_t* outputData);

#endif /* MODULES_AUDIO_PITCH_SHIFT_H_ */
