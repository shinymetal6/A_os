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
 * i2s_in.h
 *
 *  Created on: Oct 22, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_I2S_IN_H_
#define MODULES_SOUND_ENGINE_GENERATORS_I2S_IN_H_


extern uint8_t I2SIn_Register(uint8_t channel,AudioSource_TypeDef *i2s_in);
extern uint8_t I2SIn_Start(AudioSource_TypeDef *i2s_in);
extern uint8_t I2SIn_Stop(AudioSource_TypeDef *i2s_in);

#endif /* MODULES_SOUND_ENGINE_GENERATORS_I2S_IN_H_ */
