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
 * phaser.h
 *
 *  Created on: Feb 22, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_PHASER_H_
#define MODULES_AUDIO_PHASER_H_

#define MIDI_MAX				127.f 	// floating max value
#define MIDI_MAXi				127		// integer max value
#define MIDI_MID_i				64		// integer mid value

#define PHASER_SINETABLE_SIZE 	1024

#define ALPHA					(PHASER_SINETABLE_SIZE/ (2*PI))
#define PH_STAGES 				6

#define MAX_RATE		7.f // in Hz
#define MIN_RATE		0.02f // in Hz

typedef struct _PhaserTypeDef
{
	float 	old[PH_STAGES];
	float 	wet ;
	float	dmin, dmax; //range
	float 	lfoPhase;
	float 	lfoInc;
	float	a1;
	float	zm1;
} PhaserTypeDef;

extern	void Phaser_init(uint32_t Fmin,uint32_t Fmax,uint32_t SwRate,float Feedback);
extern	void Phaser_enable(void);
extern	void Phaser_disable(void);
extern	void Do_Phaser(int16_t* inputData, int16_t* outputData);


#endif /* MODULES_AUDIO_PHASER_H_ */
