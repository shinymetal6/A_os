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
 * passthrough.h
 *
 *  Created on: Feb 26, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_PASSTHROUGH_H_
#define MODULES_AUDIO_PASSTHROUGH_H_

extern	void PassThrough_init(void);
extern	void PassThrough_enable(void);
extern	void PassThrough_disable(void);
extern	void Do_PassThrough(int16_t* inputData, int16_t* outputData);


#endif /* MODULES_AUDIO_PASSTHROUGH_H_ */
