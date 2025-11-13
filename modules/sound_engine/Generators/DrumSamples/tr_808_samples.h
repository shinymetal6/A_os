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
 * tr_808_samples.h
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_808_SAMPLES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_808_SAMPLES_H_

#define 	TR_808_boom_kick_length	128
#define		TR_808_kick_length		128
#define		TR_808_snare_length		64
#define		TR_808_hh_length			32
#define		TR_808_clap_length		28

extern 		const	int16_t		TR_808_kick[];
extern 		const	int16_t		TR_808_boom_kick[];
extern 		const	int16_t 	TR_808_snare[];
extern 		const	int16_t 	TR_808_hh[];
extern 		const	int16_t 	TR_808_clap[];

#define		TR_808_NUM_VOICES	5
extern		DrumVoice_TypeDef TR_808_voices[TR_808_NUM_VOICES];
#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_TR_808_SAMPLES_H_ */
