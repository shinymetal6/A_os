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
 * sample_player.c
 *
 *  Created on: Nov 13, 2025
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef SOUND_ENGINE_ENABLED
#include "../sound_engine.h"

#include "sample_player.h"

Voice voices[MAX_VOICES] = {{0}};

void init_sample_player() {
    for (int i = 0; i < MAX_VOICES; i++) {
        voices[i].pos = 0;
        voices[i].playing = 0;
    }
}

void play_sample(uint8_t voice_id, float vol, uint8_t should_loop) {
    if (voice_id >= MAX_VOICES || !voices[voice_id].sample) return;
    voices[voice_id].playing = 1;
    voices[voice_id].pos = 0;
    voices[voice_id].volume = vol;
    voices[voice_id].loop = should_loop;
}

void stop_sample(uint8_t voice_id) {
    if (voice_id < MAX_VOICES) voices[voice_id].playing = 0;
}

void mix_audio(float* out, uint32_t blockSize) {
    for (uint32_t i = 0; i < blockSize; i++) out[i] = 0.0f;

    for (int v = 0; v < MAX_VOICES; v++) {
        if (!voices[v].playing || !voices[v].sample) continue;
        for (uint32_t i = 0; i < blockSize && voices[v].playing; i++) {
            if (voices[v].pos < voices[v].length) {
                out[i] += voices[v].sample[voices[v].pos++] * voices[v].volume / 32768.0f;
            } else if (voices[v].loop) {
                voices[v].pos = 0;
            } else {
                voices[v].playing = 0;
            }
        }
    }
}
extern	FATFS fs;
extern	FIL file;

FRESULT mount_sd(void)
{
	return f_mount(&fs, "", 1);
}

FRESULT open_wav_file(const char* fname, wav_header_t* header)
{
    FRESULT res = f_open(&file, fname, FA_READ);
    if (res != FR_OK) return res;
    UINT br;
    f_read(&file, header, sizeof(wav_header_t), &br);
    if (res != FR_OK || !is_valid_wav(header)) { f_close(&file); return FR_INVALID_OBJECT; }
    wav_data_header_t data_hdr;
    f_read(&file, &data_hdr, sizeof(data_hdr), &br);
    return FR_OK;
}
uint32_t read_audio_data(int16_t* buffer, uint32_t max_samples) {
    UINT br; uint32_t n = 0;
    while (n < max_samples) {
        uint16_t s; if (f_read(&file, &s, 2, &br) != FR_OK || br == 0) break;
        buffer[n++] = s;
    }
    return n;
}
void close_file(void) { f_close(&file); }

int is_valid_wav(wav_header_t* h) {
    return (h->riff[0] == 'R' && h->riff[1] == 'I' && h->riff[2] == 'F' && h->riff[3] == 'F' &&
            h->wave[0] == 'W' && h->wave[1] == 'A' && h->wave[2] == 'V' && h->wave[3] == 'E' &&
            h->format_type == 1 &&                // PCM
            h->channels == 1 &&                   // Mono
            h->sample_rate == 48000 &&            // 48 kHz
            h->bits_per_sample == 16);            // 16-bit
}

uint32_t get_sample_count(wav_header_t* h, wav_data_header_t* d) {
    return d->data_size / (h->channels * h->bits_per_sample / 8);
}


// Buffers
/*
float mixer_buffer[BLOCK_SIZE];
int16_t output_buffer[BLOCK_SIZE];
int16_t loaded_sample_buffer[1024 * 100]; // 100 KB
uint32_t loaded_sample_length = 0;
void sample_player_init(void)
{
    init_sample_player();
    mount_sd();

    // Load KICK.WAV from SD card
    wav_header_t header;
    if (open_wav_file("KICK.WAV", &header) == FR_OK) {
        loaded_sample_length = read_audio_data(loaded_sample_buffer, 1024*100);
        close_file();

        // Assign to voice 0
        extern Voice voices[];
        voices[0].sample = loaded_sample_buffer;
        voices[0].length = loaded_sample_length;
    }

    // Start audio and MIDI
}
void call_from_HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    mix_audio(mixer_buffer, BLOCK_SIZE/2);
    for (int i = 0; i < BLOCK_SIZE/2; i++) {
        float s = mixer_buffer[i];
        if (s > 1.0f) s = 1.0f; if (s < -1.0f) s = -1.0f;
        output_buffer[i] = (int16_t)(s * 32767.0f);
    }
}

void call_from_HAL_I2S_RxTxCpltCallback(I2S_HandleTypeDef *hi2s) {
    mix_audio(&mixer_buffer[BLOCK_SIZE/2], BLOCK_SIZE/2);
    for (int i = 0; i < BLOCK_SIZE/2; i++) {
        float s = mixer_buffer[BLOCK_SIZE/2 + i];
        if (s > 1.0f) s = 1.0f; if (s < -1.0f) s = -1.0f;
        output_buffer[BLOCK_SIZE/2 + i] = (int16_t)(s * 32767.0f);
    }
}
*/
#endif // #ifdef SOUND_ENGINE_ENABLED
