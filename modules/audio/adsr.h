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
 * adsr.h
 *
 *  Created on: Feb 23, 2024
 *      Author: fil
 */

#ifndef MODULES_AUDIO_ADSR_H_
#define MODULES_AUDIO_ADSR_H_

typedef struct _AdsrTypeDef
{
	uint8_t		state;
	uint32_t 	AttackRate;
	float	 	AttackStartLevel,AttackStartLevel_WORK;
	float	 	AttackDelta;
	float		DecayLevel,DecayDelta,DecayLevel_WORK;
	uint32_t 	DecayRate;
	float	 	SustainLevel;
	uint32_t 	ReleaseRate;
	float	 	ReleaseLevel,ReleaseDelta,ReleaseLevel_WORK;
	float	 	OutLlevel;
} AdsrTypeDef;

#define	IDLE		0
#define	ATTACK		1
#define	DECAY		2
#define	SUSTAIN		3
#define	RELEASE		4

extern	void Adsr_set(uint32_t AttackRate,uint32_t AttackStartLevel,uint32_t DecayRate,uint32_t SustainLevel,uint32_t ReleaseRate);
extern	void Do_Adsr_sm(void);
extern	void Adsr_init(uint32_t AttackRate,uint32_t AttackStartLevel,uint32_t DecayRate,uint32_t SustainLevel,uint32_t ReleaseRate);
extern	void Adsr_enable(void);
extern	void Adsr_disable(void);


#endif /* MODULES_AUDIO_ADSR_H_ */
