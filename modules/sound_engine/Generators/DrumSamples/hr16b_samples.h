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
 * hr16b_samples.h
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_HR16B_SAMPLES_H_
#define MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_HR16B_SAMPLES_H_

#define 	HR_16B_kick_length 		44
#define		HR_16B_snare_length		50
#define		HR_16B_low_tom_length	33
#define		HR_16B_mid_tom_length	30
#define		HR_16B_high_tom_length	29
#define		HR_16B_closed_hh_length	25
#define		HR_16B_open_hh_length	29
#define		HR_16B_clap_length		29
#define		HR_16B_crash_length		32
#define		HR_16B_rimshot_length	24

extern 		const	int16_t		HR_16B_kick[];
extern 		const	int16_t		HR_16B_snare[];
extern 		const	int16_t 	HR_16B_low_tom[];
extern 		const	int16_t 	HR_16B_mid_tom[];
extern 		const	int16_t 	HR_16B_high_tom[];
extern 		const	int16_t 	HR_16B_closed_hh[];
extern 		const	int16_t 	HR_16B_open_hh[];
extern 		const	int16_t 	HR_16B_clap[];
extern 		const	int16_t 	HR_16B_rimshot[];
extern 		const	int16_t 	HR_16B_crash[];

#define		HR_16B_NUM_VOICES	10
extern		DrumVoice_TypeDef HR_16B_voices[HR_16B_NUM_VOICES];

#endif /* MODULES_SOUND_ENGINE_GENERATORS_DRUMSAMPLES_HR16B_SAMPLES_H_ */
