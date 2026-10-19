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
	uint16_t 			*dac_buffer;
	uint16_t 			channel;
	uint16_t 			dac_buf_size;
	uint16_t 			alignment;
	float				sample_rate;
	float				volume; /* 0 .. 1 */
	uint32_t			PSC;
	uint32_t			ARR;
	uint32_t 			wakeup_id;
}DAC_DriverStruct_t;
/* status */
#define		DAC_STATUS_FULL			0x01
#define		DAC_STATUS_STEREO		0x20
#define		DAC_STATUS_DATA_READY	0x40
#define		DAC_STATUS_RUNNING		0x80
/* flags */
#define		DAC_FLAGS_WAKEUP			0x80


#define		DEFAULT_SAMPLE_FREQUENCY	44100

extern	uint32_t dac_register(DAC_DriverStruct_t *dac);
extern	uint32_t dac_start(DAC_DriverStruct_t *dac);
extern	uint32_t dac_stop(DAC_DriverStruct_t *dac);
extern	uint32_t dac_get_status(DAC_DriverStruct_t *dac);
extern	uint32_t dac_init(DAC_DriverStruct_t *dac);
extern	uint32_t dac_play_wav(DAC_DriverStruct_t *dac,uint16_t *wav_ptr);
extern	uint32_t dac_stop_wav(DAC_DriverStruct_t *dac);
extern  uint32_t dac_timer_set(DAC_DriverStruct_t *dac_drv,float frequency);


#endif /* DRIVERS_ANALOG_DAC_DAC_DRIVER_H_ */
