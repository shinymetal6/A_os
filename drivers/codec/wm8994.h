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
 * wm8994.h
 *
 *  Created on: Feb 26, 2024
 *      Author: fil
 */

#ifndef DRIVERS_CODEC_WM8994_H_
#define DRIVERS_CODEC_WM8994_H_

#define CODEC_WM8994_I2C_ADDRESS	0x34
#define	WM8994_ID					0x8994
#define WM8994_CHIPID_ADDR			0x00
#define VOLUME_CONVERT(Volume)        (((Volume) > 100)? 100:((uint8_t)(((Volume) * 63) / 100)))
#define VOLUME_IN_CONVERT(Volume)     (((Volume) >= 100)? 239:((uint8_t)(((Volume) * 240) / 100)))
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0

#endif /* DRIVERS_CODEC_WM8994_H_ */
