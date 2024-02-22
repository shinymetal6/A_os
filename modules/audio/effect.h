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
 * effect.h
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_EFFECT_H_
#define MODULES_AUDIO_EFFECT_H_

typedef struct
{
	char name[40];
	float tick;
	float min;
	float max;
}Parameter;

typedef struct
{
	int on;
	char name[40];
	int currentParam;
	int paramNum;
	Parameter* parameters;
	float* paramValues;
	void (*processBuffer)(
			uint16_t* inputData,
			uint16_t* outputData,
			uint32_t offset);
}Effect;

#endif /* MODULES_AUDIO_EFFECT_H_ */
