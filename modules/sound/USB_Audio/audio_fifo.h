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
 * audio_fifo.h
 *
 *  Created on: May 23, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_USB_AUDIO_AUDIO_FIFO_H_
#define MODULES_SOUND_USB_AUDIO_AUDIO_FIFO_H_


#include <stdint.h>

// Define boolean replacements
typedef uint8_t bool;
#define true  1
#define false 0

typedef struct {
    int16_t *buffer;
    uint16_t head;
    uint16_t tail;
    uint16_t fifo_count;
    uint16_t size;
    uint8_t full; // 1 if full, 0 otherwise
} AudioFifo_TypeDef;

#endif /* MODULES_SOUND_USB_AUDIO_AUDIO_FIFO_H_ */
