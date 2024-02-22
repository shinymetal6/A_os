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
 * reverb.h
 *
 *  Created on: Feb 21, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_REVERB_H_
#define MODULES_AUDIO_REVERB_H_

//Schroeder delays from 25k->96k interpolated
//*2 delay extension -> not more possible without external ram
#define l_CB0 3460*2
#define l_CB1 2988*2
#define l_CB2 3882*2
#define l_CB3 4312*2
#define l_AP0 480*2
#define l_AP1 161*2
#define l_AP2 46*2

typedef struct _ReverbTypeDef
{
	uint32_t 	cf0_lim, cf1_lim, cf2_lim, cf3_lim, ap0_lim, ap1_lim, ap2_lim;
	float 	wet , time;
	float 	apbuf0[l_AP0], apbuf1[l_AP1], apbuf2[l_AP2];
	float 	cf0_g, cf1_g, cf2_g, cf3_g;
	float 	ap0_g, ap1_g, ap2_g;
	uint32_t 	cf0_p, cf1_p, cf2_p, cf3_p, ap0_p, ap1_p, ap2_p;
} ReverbTypeDef;

extern	void Reverb_init(void);
extern	void Reverb_enable(void);
extern	void Reverb_disable(void);
extern	void Do_Reverb(int16_t* inputData, int16_t* outputData);




#endif /* MODULES_AUDIO_REVERB_H_ */
