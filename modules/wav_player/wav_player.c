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
 * wav_player.c
 *
 *  Created on: Oct 19, 2026
 *      Author: fil
 */

#include "main.h"

#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef A_OS_WAV_PLAYER_ENABLED

#include "wav_player.h"
#include <string.h>

#include "ff.h"
#include "fatfs.h"

extern	FIL wav_file;
extern	uint8_t sdMounted;
extern	FATFS USERFATFS;  /* File system object for USER logical drive */
extern	char USERPath[4]; /* USER logical drive path */

ITCM_AREA_CODE uint32_t WAV_Player_Init(DAC_DriverStruct_t *dac_drv,char* filename)
{
UINT header_bytes_read;
uint32_t sample_rate;
uint16_t num_channels;
uint16_t bits_per_sample;
UINT	bytes_read;

    if (f_open(&wav_file, filename, FA_READ) != FR_OK)
    	return 1;

    f_lseek(&wav_file, 22);
    f_read(&wav_file, &num_channels, 2, &header_bytes_read);
    if ( num_channels != 1 )
    	return 1;

    // Offset 24 (4 bytes): Sampling Frequency / Rate
    f_read(&wav_file, &sample_rate, 4, &header_bytes_read);
    dac_drv->sample_rate = (float )sample_rate;

    // Offset 34 (2 bytes): Bits per sample (e.g., 16-bit PCM)
    f_lseek(&wav_file, 34);
    f_read(&wav_file, &bits_per_sample, 2, &header_bytes_read);
    if ( bits_per_sample != 16 )
    	return 1;
    dac_timer_set(dac_drv,dac_drv->sample_rate);
    f_lseek(&wav_file, 44);

    // 3. Pre-fill the entire buffer before starting the DAC
    f_read(&wav_file, &dac_drv->dac_buffer[0], dac_drv->dac_buf_size * 2, &bytes_read);

    // Process raw 16-bit signed PCM data to 12-bit unsigned for the DAC
    for(int i=0; i<dac_drv->dac_buf_size; i++)
    {
        int16_t sample = ((int16_t*)dac_drv->dac_buffer)[i];
        // Shift 16-bit signed (-32768 to 32767) to 12-bit unsigned (0 to 4095)
        dac_drv->dac_buffer[i] = (uint16_t)((sample + 32768) >> 4);
    }

    dac_start(dac_drv);
    return 0;
}

ITCM_AREA_CODE uint32_t WAV_Player_Process(DAC_DriverStruct_t *dac_drv)
{
UINT	bytes_read;
uint16_t *fill_ptr = ((dac_drv->status & DAC_STATUS_FULL) == 0) ? &dac_drv->dac_buffer[0] : &dac_drv->dac_buffer[dac_drv->dac_buf_size / 2];
	if (f_read(&wav_file, fill_ptr, dac_drv->dac_buf_size, &bytes_read) == FR_OK)
	{
		if (bytes_read == 0)
		{
			dac_stop(dac_drv);
			f_close(&wav_file);
			return 1;
		}
		int16_t *pcm_ptr = (int16_t*)fill_ptr;
		for(int i = 0; i < (dac_drv->dac_buf_size / 2); i++)
		{
			if ( i > bytes_read)
				fill_ptr[i] = 2047;
			else
			{
				fill_ptr[i] = (uint16_t)((pcm_ptr[i] + 32768) >> 4);
				fill_ptr[i] = (uint16_t)((float)fill_ptr[i]*dac_drv->volume);
			}
		}
	    return 0;
	}
	else
	{
		dac_stop(dac_drv);
		f_close(&wav_file);
	}
    return 1;
}

#endif //#ifdef A_OS_WAV_PLAYER_ENABLED
