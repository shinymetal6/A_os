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
 * wah.h
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_WAH_H_
#define MODULES_AUDIO_WAH_H_

typedef struct _WahTypeDef
{
	int 	WtrP;
	float 	Rd_P;
	float 	Shift;
	float 	CrossFade;
	float 	a0, a1, a2, b1, b2, hp_in_z1, hp_in_z2, hp_out_z1, hp_out_z2;

} WahTypeDef;

extern	void Wah_init(uint32_t CenterFrequency,uint32_t Rate,uint32_t Depth);
extern	void Wah_enable(void);
extern	void Wah_disable(void);
extern	void Do_Wah(int16_t* inputData, int16_t* outputData);

#endif /* MODULES_AUDIO_WAH_H_ */
