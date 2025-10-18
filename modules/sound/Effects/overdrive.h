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
 * overdrive.h
 *
 *  Created on: Oct 16, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_OVERDRIVE_H_
#define MODULES_SOUND_EFFECTS_OVERDRIVE_H_

typedef struct
{
	uint8_t		status;
	uint8_t		flags;
	uint16_t	*overdrive;
	float		f_overdrive;
}OVERDRIVE_Effect_TypeDef;

extern void Effect_Overdrive_Init(uint32_t *effect_s);
extern void Effect_Overdrive(uint32_t *effect_s, uint32_t start_sample);

#endif /* MODULES_SOUND_EFFECTS_OVERDRIVE_H_ */
