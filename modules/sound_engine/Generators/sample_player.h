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
 * sample_player.h
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#ifndef MODULES_SOUND_ENGINE_GENERATORS_SAMPLE_PLAYER_H_
#define MODULES_SOUND_ENGINE_GENERATORS_SAMPLE_PLAYER_H_

#include <stdint.h>
//#include "ff.h"
#include "../../fat/FatFs/ff.h"

#define BLOCK_SIZE      64
#define MAX_VOICES      4

typedef struct {
    const int16_t* sample;
    uint32_t length;
    uint32_t pos;
    float volume;
    uint8_t playing;
    uint8_t loop;
} Voice;

#pragma pack(push, 1)
typedef struct {
    uint8_t  riff[4];           // "RIFF"
    uint32_t overall_size;
    uint8_t  wave[4];           // "WAVE"
    uint8_t  fmt_chunk_marker[4]; // "fmt "
    uint32_t length_of_fmt;
    uint16_t format_type;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byterate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    // Extended fmt bytes may follow
} wav_header_t;

typedef struct {
    uint8_t  data_chunk_marker[4]; // "data"
    uint32_t data_size;             // Size of audio data
} wav_data_header_t;
#pragma pack(pop)

extern	int is_valid_wav(wav_header_t* h);
extern	uint32_t get_sample_count(wav_header_t* h, wav_data_header_t* d);
extern	FATFS fs;
extern	FIL file;
extern	FRESULT mount_sd(void);
extern	FRESULT open_wav_file(const char* fname, wav_header_t* header);



#endif /* MODULES_SOUND_ENGINE_GENERATORS_SAMPLE_PLAYER_H_ */
