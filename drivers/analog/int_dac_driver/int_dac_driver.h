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
 * int_dac_driver.h
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_INT_DAC_DRIVER_INT_DAC_DRIVER_H_
#define DRIVERS_ANALOG_INT_DAC_DRIVER_INT_DAC_DRIVER_H_

#ifdef A_OS_DAC_ENABLED

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	DAC_HandleTypeDef 	*dac;
	TIM_HandleTypeDef 	*dac_timer;
	int16_t 			*dac_buffer;
	uint16_t 			channel;
	uint16_t 			len;
	uint16_t 			alignment;
	uint32_t			dac_sample_frequency;
	uint32_t			PSC;
	uint32_t			ARR;
	uint8_t				dac_wav_flags;
	int16_t				*wav_ptr;
	uint32_t			wav_samples_counter;
	uint32_t			wav_len;
	uint16_t			wav_volume_int;
	uint8_t				wav_flags;
	int16_t				wav_progressive_sample;
}DAC_Drv_TypeDef;

/* status */
#define		DAC_STATUS_HALF			0x01
#define		DAC_STATUS_FULL			0x02
#define		DAC_STATUS_STEREO		0x20
#define		DAC_STATUS_DATA_READY	0x40
#define		DAC_STATUS_RUNNING		0x80
/* flags */
#define		DAC_FLAGS_WAKEUP			0x80
#define		DAC_FLAGS_USE_SYNTHMODULE	0x02
#define		DAC_FLAGS_USE_AUDIOMODULE	0x01
/* wav_flags */
#define		DAC_WAV_FLAGS_8000		0x01
#define		DAC_WAV_FLAGS_11025		0x02
#define		DAC_WAV_FLAGS_16000		0x04
#define		DAC_WAV_FLAGS_22050		0x08
#define		DAC_WAV_FLAGS_DO_PLAY	0x80

/*
 * [Master RIFF chunk]
   FileTypeBlocID  (4 bytes) : Identifier « RIFF »  (0x52, 0x49, 0x46, 0x46)
   FileSize        (4 bytes) : Overall file size minus 8 bytes
   FileFormatID    (4 bytes) : Format = « WAVE »  (0x57, 0x41, 0x56, 0x45)

[Chunk describing the data format]
   FormatBlocID    (4 bytes) : Identifier « fmt␣ »  (0x66, 0x6D, 0x74, 0x20)
   BlocSize        (4 bytes) : Chunk size minus 8 bytes, which is 16 bytes here  (0x10)
   AudioFormat     (2 bytes) : Audio format (1: PCM integer, 3: IEEE 754 float)
   NbrChannels     (2 bytes) : Number of channels
   Frequency       (4 bytes) : Sample rate (in hertz)
   BytePerSec      (4 bytes) : Number of bytes to read per second (Frequency * BytePerBloc).
   BytePerBloc     (2 bytes) : Number of bytes per block (NbrChannels * BitsPerSample / 8).
   BitsPerSample   (2 bytes) : Number of bits per sample

[Chunk containing the sampled data]
   DataBlocID      (4 bytes) : Identifier « data »  (0x64, 0x61, 0x74, 0x61)
   DataSize        (4 bytes) : SampledData size
   */

typedef struct
{
	uint8_t				FileTypeBlocID[4];
	uint32_t			FileSize;
	uint8_t				FileFormatID[4];
	uint8_t				FormatBlocID[4];
	uint32_t			BlocSize;
	uint16_t			AudioFormat;
	uint16_t			NbrChannels;
	uint32_t			Frequency;
	uint32_t			BytePerSec;
	uint16_t			BytePerBloc;
	uint16_t			BitsPerSample;
	uint8_t				DataBlocID[4];
	uint32_t			DataSize;
	int16_t			 	first_audio_sample;
}Wav_Header_TypeDef;

#define	STD_DAC_PRESCALER 10

extern uint32_t	int_dac_register(DAC_Drv_TypeDef *analog_driver_private_data);
#endif // #ifdef A_OS_DAC_ENABLED

#endif /* DRIVERS_ANALOG_INT_DAC_DRIVER_INT_DAC_DRIVER_H_ */
