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
 * vca.h
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_VCA_H_
#define MODULES_AUDIO_VCA_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	float	 			volume;
}VCA_Effect_TypeDef;
/* flags */

extern	void Do_Vca(int16_t* inputData, int16_t* outputData,uint8_t effect_index);


#endif /* MODULES_AUDIO_VCA_H_ */
