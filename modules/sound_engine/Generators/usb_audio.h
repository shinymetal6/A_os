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
 * usb_audio.h
 *
 *  Created on: Dec 9, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_USB_AUDIO_H_
#define MODULES_SOUND_ENGINE_GENERATORS_USB_AUDIO_H_

#ifdef USB_DEVICE_ENABLED

#include "audio_sources.h"

extern uint8_t USBAudio_Register(AUDIO_Source_TypeDef *i2s_in);
/*
extern uint8_t I2SIO_Start(AUDIO_Source_TypeDef *i2s_in);
extern uint8_t I2SIO_Stop(AUDIO_Source_TypeDef *i2s_in);
*/
#endif // #ifdef USB_DEVICE_ENABLED

#endif /* MODULES_SOUND_ENGINE_GENERATORS_USB_AUDIO_H_ */
