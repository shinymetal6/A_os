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
 * audio.h
 *
 *  Created on: Jan 8, 2024
 *      Author: fil
 */

#ifndef KERNEL_AUDIO_H_
#define KERNEL_AUDIO_H_

typedef struct
{
	int16_t		channel[2];
}WaveLR_t;

#define	AUDIO_LEFT_CH		0
#define	AUDIO_RIGHT_CH		1

#endif /* KERNEL_AUDIO_H_ */
