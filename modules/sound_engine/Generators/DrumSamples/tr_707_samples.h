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
 * tr_707_samples.h
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_707_SAMPLES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_707_SAMPLES_H_

#define 	TR_707_kick_length 		54
#define		TR_707_snare_length		52
#define		TR_707_low_tom_length	32
#define		TR_707_mid_tom_length	29
#define		TR_707_high_tom_length	29
#define		TR_707_closed_hh_length	25
#define		TR_707_open_hh_length	29
#define		TR_707_clap_length		29
#define		TR_707_rimshot_length	24
#define		TR_707_cowbell_length	39

extern 		const	int16_t		TR_707_kick[];
extern 		const	int16_t		TR_707_snare[];
extern 		const	int16_t 	TR_707_low_tom[];
extern 		const	int16_t 	TR_707_mid_tom[];
extern 		const	int16_t 	TR_707_high_tom[];
extern 		const	int16_t 	TR_707_closed_hh[];
extern 		const	int16_t 	TR_707_open_hh[];
extern 		const	int16_t 	TR_707_clap[];
extern 		const	int16_t 	TR_707_rimshot[];
extern 		const	int16_t 	TR_707_cowbell[];

#define		TR_707_NUM_VOICES	10
extern		DrumVoice_TypeDef TR_707_voices[TR_707_NUM_VOICES];

#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_707_SAMPLES_H_ */
