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
 * audio_fifo.c
 *
 *  Created on: May 23, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../sound.h"

#define	USB_AUDIO_FIFO 1
#ifdef SOUND_ENABLED
#ifdef	USB_AUDIO
#ifdef	USB_AUDIO_FIFO
#include "audio_fifo.h"

#define FIFOSIZE	32768
int16_t	fifo_buffer[32768];
AudioFifo_TypeDef	AudioFifo =
{
		.buffer = fifo_buffer,
		.size = FIFOSIZE,
};

uint8_t audio_fifo_is_empty(AudioFifo_TypeDef *f)
{
    return (!f->full && (f->head == f->tail));
}

uint8_t audio_fifo_is_full(AudioFifo_TypeDef *f)
{
    return f->full;
}

uint16_t audio_fifo_count(AudioFifo_TypeDef *f)
{
	return f->fifo_count;
}

uint8_t audio_fifo_push(AudioFifo_TypeDef *f, int16_t data)
{
    if (f->fifo_count == f->size)
        return false;

    f->buffer[f->head] = data;
    f->head = (f->head + 1) % f->size;

    if (f->head == f->tail)
        f->full = true;
    f->fifo_count ++;
    return true;
}

uint8_t audio_fifo_pop(AudioFifo_TypeDef *f, int16_t *data)
{
    if (f->fifo_count == 0)
        return false;

    *data = f->buffer[f->tail];
    f->tail = (f->tail + 1) % f->size;
    f->full = false;
    f->fifo_count --;

    return true;
}

#endif // #ifdef	USB_AUDIO_FIFO
#endif //#ifdef	USB_AUDIO
#endif // #ifdef SOUND_ENABLED
