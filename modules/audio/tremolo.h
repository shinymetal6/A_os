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
 * tremolo.h
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_TREMOLO_H_
#define MODULES_AUDIO_TREMOLO_H_

extern	void Tremolo_init(uint32_t Rate,uint32_t Depth);
extern	void Tremolo_enable(void);
extern	void Tremolo_disable(void);
extern	void Do_Tremolo(int16_t* inputData, int16_t* outputData);

#endif /* MODULES_AUDIO_TREMOLO_H_ */
