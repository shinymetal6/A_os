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

Sampler_Voices_TypeDef voices[MAX_VOICES] = {{0}};

void Sample_Player_Play_Sample(uint8_t voice_id, float vol, uint8_t should_loop)
{
    if (voice_id >= MAX_VOICES || !voices[voice_id].sample)
    	return;
    voices[voice_id].playing = 1;
    voices[voice_id].pos = 0;
    voices[voice_id].volume = vol;
    voices[voice_id].loop = should_loop;
}

void Sample_Player_Stop_Sample(uint8_t voice_id)
{
    if (voice_id < MAX_VOICES)
    	voices[voice_id].playing = 0;
}

void Sample_Player_Mix_Audio(float* out, uint32_t blockSize)
{
    for (uint32_t i = 0; i < blockSize; i++)
    	out[i] = 0.0f;

    for (int v = 0; v < MAX_VOICES; v++)
    {
        if (!voices[v].playing || !voices[v].sample)
        	continue;
        for (uint32_t i = 0; i < blockSize && voices[v].playing; i++)
        {
            if (voices[v].pos < voices[v].length)
            {
                out[i] += voices[v].sample[voices[v].pos++] * voices[v].volume / 32768.0f;
            }
            else if (voices[v].loop)
            {
                voices[v].pos = 0;
            }
            else
            {
                voices[v].playing = 0;
            }
        }
    }
}

extern	FATFS fs;
extern	FIL file;

FRESULT Sample_Player_Mount_SD(void)
{
	return f_mount(&fs, "", 1);
}

int Sample_Player_Is_Valid_Wav(Wav_Hdr_TypeDef* h)
{
    return (h->riff[0] == 'R' && h->riff[1] == 'I' && h->riff[2] == 'F' && h->riff[3] == 'F' &&
            h->wave[0] == 'W' && h->wave[1] == 'A' && h->wave[2] == 'V' && h->wave[3] == 'E' &&
            h->format_type == 1 &&                // PCM
            h->channels == 1 &&                   // Mono
            h->sample_rate == 48000 &&            // 48 kHz
            h->bits_per_sample == 16);            // 16-bit
}

FRESULT Sample_Player_Open_Wav_File(SamplePlayer_TypeDef *sampler,char* fname)//, Wav_Hdr_TypeDef* header)
{
    FRESULT res = f_open(&file, fname, FA_READ);
    if (res != FR_OK)
    	return res;
    sprintf(sampler->fname,"%s",fname);
    UINT br;
    f_read(&file, &sampler->wav_header, sizeof(Wav_Hdr_TypeDef), &br);
    if (res != FR_OK || !is_valid_wav(&sampler->wav_header))
    {
    	f_close(&file);
    	return FR_INVALID_OBJECT;
    }
    f_read(&file, &sampler->wav_data, sizeof(Wav_Data_Header_TypeDef), &br);
    return FR_OK;
}

uint32_t Sample_Player_Read_Audio_Data(int16_t* buffer, uint32_t max_samples) {
    UINT br; uint32_t n = 0;
    while (n < max_samples) {
        uint16_t s;
        if (f_read(&file, &s, 2, &br) != FR_OK || br == 0)
        	break;
        buffer[n++] = s;
    }
    return n;
}

void Sample_Player_Close_File(void)
{
	f_close(&file);
}

uint32_t Sample_Player_Get_sample_count(Wav_Hdr_TypeDef* h, Wav_Data_Header_TypeDef* d)
{
    return d->data_size / (h->channels * h->bits_per_sample / 8);
}

ITCM_AREA_CODE uint8_t Sample_Player_Register(AUDIO_Source_TypeDef *sample_player)
{
	if ( sample_player->out_buf == NULL )
		return 1;
	if ( sample_player->ptr_gen_struct == NULL )	// pointer to the user space drum_voices drum machine
		return 1;
	if ( sample_player->channel_in == AUDIO_SOURCE_LEFT)
	{
		if ( AudioSourceLeft == NULL )
		{
			AudioSourceLeft = sample_player;
			sample_player->next_source = NULL;
		}
		else
		{
			AUDIO_Source_TypeDef *source = AudioSourceLeft;
			while(source->next_source != NULL)
				source = (AUDIO_Source_TypeDef *)source->next_source;
			source->next_source = (uint32_t *)sample_player;
			sample_player->next_source = NULL;
		}
	}
	sample_player->block_size = I2S_EFFECT_SIZE;
	SamplePlayer_TypeDef *sampler = (SamplePlayer_TypeDef *)sample_player->ptr_gen_struct;
	if ( strlen(sampler->fname) == 0 )
		return 1;
    for (int i = 0; i < MAX_VOICES; i++) {
    	sampler->sampler_voices[i].pos = 0;
    	sampler->sampler_voices[i].playing = 0;
    }
	sample_player->source_type = SOUND_SOURCE_IS_SAMPLE_PLAYER;
	return 0;
}

#endif // #ifdef SOUND_ENGINE_ENABLED
