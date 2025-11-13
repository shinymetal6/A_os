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
 * linndrum_lm1_samples.h
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_LINNDRUM_LM1_SAMPLES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_LINNDRUM_LM1_SAMPLES_H_

#define 	LM1_kick_length 		45
#define		LM1_snare_length		68
#define		LM1_low_tom_length		33
#define		LM1_mid_tom_length		30
#define		LM1_high_tom_length		29
#define		LM1_closed_hh_length	25
#define		LM1_open_hh_length		29
#define		LM1_clap_length			29
#define		LM1_crash_length		22
#define		LM1_rimshot_length		24

extern 		const	int16_t		LM1_kick[];
extern 		const	int16_t		LM1_snare[];
extern 		const	int16_t 	LM1_low_tom[];
extern 		const	int16_t 	LM1_mid_tom[];
extern 		const	int16_t 	LM1_high_tom[];
extern 		const	int16_t 	LM1_closed_hh[];
extern 		const	int16_t 	LM1_open_hh[];
extern 		const	int16_t 	LM1_clap[];
extern 		const	int16_t 	LM1_rimshot[];
extern 		const	int16_t 	LM1_crash[];

#define		LM1_NUM_VOICES	10
extern		DrumVoice_TypeDef LM1_voices[LM1_NUM_VOICES];

#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_LINNDRUM_LM1_SAMPLES_H_ */
