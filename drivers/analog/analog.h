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
 * analog.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ANALOG_ANALOG_H_
#define DRIVERS_ANALOG_ANALOG_H_

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint32_t			*analog_driver_private_data;
	uint32_t			(*adc_start)  (uint8_t handle);
	uint32_t			(*adc_stop)  (uint8_t handle);
	uint32_t			(*adc_get_status) (uint8_t handle);
	uint32_t			(*adc_init) (uint8_t handle);
	uint32_t			(*dac_start)  (uint8_t handle);
	uint32_t			(*dac_stop)  (uint8_t handle);
	uint32_t			(*dac_get_status) (uint8_t handle);
	uint32_t			(*dac_init) (uint8_t handle);
	uint32_t			(*i2s_start)  (uint8_t handle);
	uint32_t			(*i2s_stop)  (uint8_t handle);
	uint32_t			(*i2s_get_status) (uint8_t handle);
	uint32_t			(*i2s_init) (uint8_t handle);
	uint32_t			(*codec_start)  (uint8_t handle);
	uint32_t			(*codec_stop)  (uint8_t handle);
	uint32_t			(*codec_get_status) (uint8_t handle);
	uint32_t			(*codec_init) (uint8_t handle);
	uint32_t			(*codec_internal_ops) (uint8_t handle,uint8_t command,uint32_t param0,uint32_t param1,uint32_t param2,uint32_t param3);
}ANALOG_DriverStruct_t;

#ifdef STM32H7xx_HAL_ADC_H
#include "int_adc_driver/int_adc_driver.h"
#endif

#ifdef STM32H7xx_HAL_DAC_H
#include "int_dac_driver/int_dac_driver.h"
#endif

#ifdef STM32H7xx_HAL_I2S_H
	#include "int_i2s_driver/int_i2s_driver.h"
	#ifdef STM32H7xx_HAL_I2C_H
		#include "codec/nau88c22.h"
	#endif // #ifdef STM32H7xx_HAL_I2C_H
#endif // #ifdef STM32H7xx_HAL_I2S_H

extern	uint32_t adc_start(uint8_t handle);
extern	uint32_t adc_stop(uint8_t handle);
extern	uint32_t adc_get_status(uint8_t handle);
extern	uint32_t adc_init(uint8_t handle);
extern	uint32_t dac_start(uint8_t handle);
extern	uint32_t dac_stop(uint8_t handle);
extern	uint32_t dac_get_status(uint8_t handle);
extern	uint32_t dac_init(uint8_t handle);
extern	uint32_t codec_start(uint8_t handle);
extern	uint32_t codec_stop(uint8_t handle);
extern	uint32_t codec_get_status(uint8_t handle);
extern	uint32_t codec_init(uint8_t handle);
extern	uint32_t codec_internal_ops(uint8_t handle,uint8_t command,uint32_t param0,uint32_t param1,uint32_t param2,uint32_t param3);

#endif /* DRIVERS_ANALOG_ANALOG_H_ */
