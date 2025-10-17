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
 * analog.c
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

SYSTEM_RAM	ANALOG_DriverStruct_t	ANALOG_DriverStruct[MAX_ANALOG_DRIVERS];
SYSTEM_RAM	uint8_t					last_analog_used_handle=0,analog_driver_request = 0;

#ifdef A_OS_ADC_ENABLED

ITCM_AREA_CODE uint32_t	adc_register(ADC_Drv_TypeDef *private_data)
{
	return int_adc_register(private_data);
}

ITCM_AREA_CODE uint32_t adc_start(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].adc_start != NULL )
		return ANALOG_DriverStruct[handle].adc_start(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t adc_stop(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].adc_stop != NULL )
		return ANALOG_DriverStruct[handle].adc_stop(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t adc_get_status(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].adc_get_status != NULL )
		return ANALOG_DriverStruct[handle].adc_get_status(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t adc_init(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].adc_init != NULL )
		return ANALOG_DriverStruct[handle].adc_init(handle);
	return 1;
}
#endif

#ifdef A_OS_DAC_ENABLED

ITCM_AREA_CODE uint32_t	dac_register(DAC_Drv_TypeDef *private_data)
{
	return int_dac_register(private_data);
}

ITCM_AREA_CODE uint32_t dac_start(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].dac_start != NULL )
		return ANALOG_DriverStruct[handle].dac_start(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t dac_stop(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].dac_stop != NULL )
		return ANALOG_DriverStruct[handle].dac_stop(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t dac_get_status(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].dac_get_status != NULL )
		return ANALOG_DriverStruct[handle].dac_get_status(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t dac_init(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].dac_init != NULL )
		return ANALOG_DriverStruct[handle].dac_init(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t dac_play_wav(uint8_t handle,uint16_t *wav_ptr)
{
	if ( ANALOG_DriverStruct[handle].dac_play_wav != NULL )
	{
		return ANALOG_DriverStruct[handle].dac_play_wav(handle,wav_ptr);
	}
	return 1;
}

ITCM_AREA_CODE uint32_t dac_stop_wav(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].dac_stop_wav != NULL )
		return ANALOG_DriverStruct[handle].dac_stop_wav(handle);
	return 1;
}
#endif

#ifdef A_OS_I2S_ENABLED

ITCM_AREA_CODE uint32_t	i2s_register(I2S_Drv_TypeDef *private_data)
{
	return int_i2s_register(private_data);
}

ITCM_AREA_CODE uint32_t i2s_start(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].i2s_start != NULL )
		return ANALOG_DriverStruct[handle].i2s_start(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t i2s_init(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].i2s_init != NULL )
		return ANALOG_DriverStruct[handle].i2s_init(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t	nau88c22_codec_register(Nau88C22_Drv_TypeDef *private_data)
{
	return int_nau88c22_codec_register(private_data);
}

ITCM_AREA_CODE uint32_t codec_start(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].codec_start != NULL )
		return ANALOG_DriverStruct[handle].codec_start(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t codec_stop(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].codec_stop != NULL )
		return ANALOG_DriverStruct[handle].codec_stop(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t codec_get_status(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].codec_get_status != NULL )
		return ANALOG_DriverStruct[handle].codec_get_status(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t codec_init(uint8_t handle)
{
	if ( ANALOG_DriverStruct[handle].codec_init != NULL )
		return ANALOG_DriverStruct[handle].codec_init(handle);
	return 1;
}

ITCM_AREA_CODE uint32_t codec_internal_ops(uint8_t handle,uint8_t command,uint32_t param0,uint32_t param1,uint32_t param2,uint32_t param3)
{
	if ( ANALOG_DriverStruct[handle].codec_internal_ops != NULL )
		return ANALOG_DriverStruct[handle].codec_internal_ops(handle,command,param0,param1,param2,param3);
	return 1;
}
#endif

