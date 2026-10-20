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
 * wav_player.h
 *
 *  Created on: Oct 19, 2026
 *      Author: fil
 */

#ifndef MODULES_WAV_PLAYER_WAV_PLAYER_H_
#define MODULES_WAV_PLAYER_WAV_PLAYER_H_

#ifdef A_OS_WAV_PLAYER_ENABLED
extern	uint32_t WAV_Player_Init(DAC_DriverStruct_t *dac_drv,char* filename);
extern	uint32_t WAV_Player_Process(DAC_DriverStruct_t *dac_drv);
#endif

#endif /* MODULES_WAV_PLAYER_WAV_PLAYER_H_ */
