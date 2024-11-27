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
 * vca_s.h
 *
 *  Created on: Feb 27, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_VCA_S_H_
#define MODULES_AUDIO_VCA_S_H_

extern	void Vca_s_init(uint8_t Volume);
extern	void Vca_s_enable(void);
extern	void Vca_s_disable(void);
extern	int16_t Do_Vca_s(int16_t inputData);
extern	void Vca_s_setMasterVolume(uint8_t Volume);

#endif /* MODULES_AUDIO_VCA_S_H_ */
