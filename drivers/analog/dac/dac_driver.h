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
 * dac_driver.h
 *
 *  Created on: Oct 29, 2025
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_DAC_DAC_DRIVER_H_
#define DRIVERS_ANALOG_DAC_DAC_DRIVER_H_

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t			*pre_drv;
	uint32_t			*next_drv;
	DAC_HandleTypeDef 	*dac;
	TIM_HandleTypeDef 	*dac_timer;
	int16_t 			*dac_buffer;
	uint16_t 			channel;
	uint16_t 			len;
	uint16_t 			alignment;
	float				dac_sample_frequency;
	uint32_t			PSC;
	uint32_t			ARR;
	uint8_t				dac_wav_flags;
	int16_t				*wav_ptr;
	uint32_t			wav_samples_counter;
	uint32_t			wav_len;
	float				wav_volume;
	uint8_t				wav_flags;
	int16_t 			*usbaudio_buffer;
	uint32_t 			usbaudio_index;
	uint32_t 			usbaudio_size;
}DAC_DriverStruct_t;

/* status */
#define		DAC_STATUS_RUNNING		0x80

extern	uint32_t dac_register(DAC_DriverStruct_t *dac);
extern	uint32_t dac_start(DAC_DriverStruct_t *dac);
extern	uint32_t dac_stop(DAC_DriverStruct_t *dac);
extern	uint32_t dac_get_status(DAC_DriverStruct_t *dac);
extern	uint32_t dac_init(DAC_DriverStruct_t *dac);
extern	uint32_t dac_play_wav(DAC_DriverStruct_t *dac,uint16_t *wav_ptr);
extern	uint32_t dac_stop_wav(DAC_DriverStruct_t *dac);


#endif /* DRIVERS_ANALOG_DAC_DAC_DRIVER_H_ */
