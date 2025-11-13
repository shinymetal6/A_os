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
 * tr_606_samples.h
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_606_SAMPLES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_606_SAMPLES_H_

#define 	TR_606_kick_length 		68
#define		TR_606_snare_length		54
#define		TR_606_low_tom_length	34
#define		TR_606_mid_tom_length	32
#define		TR_606_high_tom_length	29
#define		TR_606_hh_length			26
#define		TR_606_rimshot_length	24

extern 		const	int16_t		TR_606_kick[];
extern 		const	int16_t		TR_606_snare[];
extern 		const	int16_t 	TR_606_low_tom[];
extern 		const	int16_t 	TR_606_mid_tom[];
extern 		const	int16_t 	TR_606_high_tom[];
extern 		const	int16_t 	TR_606_hh[];
extern 		const	int16_t 	TR_606_rimshot[];

#define		TR_606_NUM_VOICES	7
extern		DrumVoice_TypeDef TR_606_voices[TR_606_NUM_VOICES];

#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_606_SAMPLES_H_ */
