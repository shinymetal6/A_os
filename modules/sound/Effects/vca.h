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
 *  Created on: Apr 24, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_EFFECTS_VCA_H_
#define MODULES_SOUND_EFFECTS_VCA_H_
#ifdef SOUND_ENABLED

typedef struct
{
	q15_t				amplitude;
}VCA_Effect_TypeDef;

extern void Effect_VCA(uint32_t *effect_s, uint32_t start_sample);

#endif // #ifdef SOUND_ENABLED
#endif /* MODULES_SOUND_EFFECTS_VCA_H_ */
