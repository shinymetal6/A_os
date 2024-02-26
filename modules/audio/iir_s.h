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
 * iir_s.h
 *
 *  Created on: Feb 24, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_IIR_S_H_
#define MODULES_AUDIO_IIR_S_H_

typedef struct _IIR_S_TypeDef
{
	float a[3], b[3], hp_in_z1, hp_in_z2, hp_out_z1, hp_out_z2;
} IIR_S_TypeDef;

#endif /* MODULES_AUDIO_IIR_S_H_ */
