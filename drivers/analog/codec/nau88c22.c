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
 * nau88c22.c
 *
 *  Created on: Nov 22, 2024
 *      Author: fil
 */

#include "main.h"

#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#ifdef A_OS_I2C_ENABLED

#include "nau88c22.h"

extern	ANALOG_DriverStruct_t			ANALOG_DriverStruct[MAX_ANALOG_DRIVERS];
extern	uint8_t							last_analog_used_handle,analog_driver_request;

const Nau88c22_t	Nau88c22[] =
{
		{
			NAU88C22_POWER_MANAGEMENT_1,/// pll off
			0x01ff
		},
		{
			NAU88C22_POWER_MANAGEMENT_2,
			//0x01b3	// Enable L/R Headphone, ADC Mix/Boost, ADC
			0x01bf		// Disable L/R Headphone, ADC Mix/Boost, ADC
		},
		{
			NAU88C22_POWER_MANAGEMENT_3,
			//0x007f	// Enable L/R main mixer, DAC
			0x01ff	// Enable L/R main mixer, DAC  - disable speaker and AUX
		},
		{
			NAU88C22_AUDIO_INTERFACE,
			0x0010	// 16-bit word length, I2S format, Stereo 000010000
			//0x0008	// 16-bit word length, I2S format, Stereo 000010000
		},
		{
			NAU88C22_COMPANDING_CONTROL,
			0x0000	// Companding control and loop back mode (all disable)
		},
		{
			NAU88C22_CLOCK_CONTROL1,
			//0x01ad	 // Divide by 6, 16K
			0x0100	 // Divide by 1 master clock source, fs and bclk are input
		},
		{
			NAU88C22_CLOCK_CONTROL2,
			//0x0006	// 16K for internal filter coefficients
			0x0000	// 48K
		},
		{
			NAU88C22_GPIO_CONTROL,
			0x0000	// gpio <- all in
			//0x0004	// gpio <- pll out
			//0x0005	// gpio <- pll lock
		},
		{
			NAU88C22_DAC_CONTROL,
			0x0008	// DAC soft mute is disabled, DAC oversampling rate is 128x
		},
		{
			NAU88C22_LEFT_DAC_DIGITAL_VOLUME,
			0x01ff	// DAC left digital volume control
		},
		{
			NAU88C22_RIGHT_DAC_DIGITAL_VOLUME,
			0x01ff	// DAC right digital volume control
		},
		{
			NAU88C22_ADC_CONTROL,
			0x0108	// ADC HP filter is disabled, ADC oversampling rate is 128x
		},
		{
			NAU88C22_LEFT_ADC_DIGITAL_VOLUME,
			0x01ff	// ADC left digital volume control
		},
		{
			NAU88C22_RIGHT_ADC_DIGITAL_VOLUME,
			0x01ff	// ADC right digital volume control
		},
		{
			NAU88C22_INPUT_CONTROL,
			//0x0000	// LLIN/RLIN is not connected to PGA
			0x0044	// only LLIN/RLIN connected to PGA -> 001000100
		},
		{
			NAU88C22_LEFT_ADC_BOOST_CONTROL,
			//0x0050	// LLIN connected, and its Gain value
			0x0055	// LLIN connected, 0 dB all 001010101
		},
		{
			NAU88C22_RIGHT_ADC_BOOST_CONTROL,
			//0x0050	 // RLIN connected, and its Gain value
			0x0055	// RLIN connected, 0 dB all 001010101
		},
		{
			NAU88C22_OUTPUT_CONTROL,
			0x0002	// Thermal shutdown enable
		},
		{
			NAU88C22_LEFT_MIXER_CONTROL,
			0x0001	// Left DAC connected to LMIX
		},
		{
			NAU88C22_RIGHT_MIXER_CONTROL,
			0x0001	// Right DAC connected to RMIX
		},
		{
			NAU88C22_OUT2_MIXER_CONTROL,
			0x0001	// Left DAC connected to LMIX
		},
		{
			NAU88C22_OUT1_MIXER_CONTROL,
			0x0001	// Right DAC connected to RMIX
		},
#ifdef NAU88C22_EXT_FREQ_12MHZ
		/* 12 MHz ext clock */
		/*
			R = 98.304/12.000 = 8.192 --> 0x3126E9
			R36 0xnm8 ; integer portion of fraction, (nm represents other settings in R36)
			R37 0x00C ; highest order 6-bits of 24-bit fraction
			R38 0x093 ; middle 9-bits of 24-bit fraction
			R39 0x0E9 ; lowest order 9-bits of 24-bit fraction
		*/
		{
			NAU88C22_PLL_N,
			0x008	// 256fs IMCLK rate : 12.288Mhz internal / 12.0 Mhz external ref clk
		},
		{
			NAU88C22_PLL_K1,
			0x00c	//
		},
		{
			NAU88C22_PLL_K2,
			0x093	//
		},
		{
			NAU88C22_PLL_K3,
			0x0e9	//
		},
#endif

#ifdef NAU88C22_EXT_FREQ_12_288MHZ
		/* 12.288 MHz ext clock */
		/*
			R = 98.304/12.288 = 8 --> 0
			R36 0xnm8 ; integer portion of fraction, (nm represents other settings in R36)
			R37 0x000 ; highest order 6-bits of 24-bit fraction
			R38 0x000 ; middle 9-bits of 24-bit fraction
			R39 0x000 ; lowest order 9-bits of 24-bit fraction
		*/

#if DEFAULT_SAMPLE_FREQUENCY == 48000
		{
			NAU88C22_PLL_N,
			0x008	// 256fs IMCLK rate : 12.288Mhz internal / 12.288Mhz external ref clk
		},
#else
		{
			NAU88C22_PLL_N,
			0x004	// 256fs IMCLK rate : 12.288Mhz internal / 12.288Mhz external ref clk
		},
#endif
		{
			NAU88C22_PLL_K1,
			0x000	//
		},
		{
			NAU88C22_PLL_K2,
			0x000	//
		},
		{
			NAU88C22_PLL_K3,
			0x000	//
		},
#endif
		{
			NAU88C22_LAST_ELEMENT,
			0	// struct_terminator
		},
};

ITCM_AREA_CODE static uint8_t Nau88c22_CheckPresent(I2C_HandleTypeDef *bus,uint16_t device_address)
{
	return HAL_I2C_IsDeviceReady(bus, device_address, 5,NAU88C22_I2C_TIMEOUT);

}

ITCM_AREA_CODE static uint32_t Nau88c22_WriteReg(I2C_HandleTypeDef *bus,uint16_t device_address,uint8_t reg_address, uint16_t reg_data)
{
uint8_t i2c_data[2];
	i2c_data[0] = (reg_address << 1) | ((reg_data & 0x100 )>> 8);
	i2c_data[1] = reg_data & 0xff;
	return HAL_I2C_Mem_Write(bus, device_address, i2c_data[0], 1, &i2c_data[1], 1, NAU88C22_I2C_TIMEOUT);
}

ITCM_AREA_CODE static uint32_t nau88c22_start(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t nau88c22_stop(uint8_t handle)
{
	return 0;
}

ITCM_AREA_CODE static uint32_t nau88c22_get_status(uint8_t handle)
{
Nau88C22_Drv_TypeDef	*codec_drv = (Nau88C22_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
	return codec_drv->status;
}

ITCM_AREA_CODE uint32_t nau88c22_init(uint8_t handle)
{
Nau88C22_Drv_TypeDef	*codec_drv = (Nau88C22_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
uint8_t	i = 0;
	if ( Nau88c22_CheckPresent(codec_drv->bus , codec_drv->device_address) == 0)
	{
		if ( Nau88c22_WriteReg(codec_drv->bus , codec_drv->device_address,NAU88C22_RESET,  0) )		// reset
				return 1;
		while(Nau88c22[i].reg_addr != NAU88C22_LAST_ELEMENT )
		{
			codec_drv->shadowregs[Nau88c22[i].reg_addr] = Nau88c22[i].reg_data;
			if ( Nau88c22_WriteReg(codec_drv->bus , codec_drv->device_address,Nau88c22[i].reg_addr,  Nau88c22[i].reg_data) )
				return 1;
			i++;
		}
		task_delay(1);
		if ( Nau88c22_WriteReg(codec_drv->bus , codec_drv->device_address,NAU88C22_POWER_MANAGEMENT_1,  0x002f) )
			return 1;
	}
	return 0;
}

uint32_t nau88c22_internal_ops(uint8_t handle,uint8_t command,uint32_t adc_dac_narrow_wide,uint32_t band,uint32_t center_frequency,uint32_t gain)
{
Nau88C22_Drv_TypeDef	*codec_drv = (Nau88C22_Drv_TypeDef	*)ANALOG_DriverStruct[handle].private_data;
I2C_HandleTypeDef		*bus = codec_drv->bus;
uint16_t 				device_address = codec_drv->device_address;
/* params order
	uint8_t adc_dac,
	uint8_t band,
	uint8_t center_frequency,
	uint8_t gain
*/
/*
uint16_t	adc_dac_narrow_wide	= (uint16_t )param0;
uint8_t 	band				= (uint8_t  )param1;
uint8_t 	center_frequency	= (uint8_t  )param2;
uint8_t 	gain				= (uint8_t  )param3;
*/
	switch(command)
	{
	case	NAU88C22_INTOP_SET_BAND1EQ:


		if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
			return 1;
		codec_drv->adc_dac_path = (adc_dac_narrow_wide & 0x01) << 8;
		codec_drv->band1_frequency = center_frequency << 5;
		codec_drv->band1_gain = gain;
		break;
	case	NAU88C22_INTOP_SET_BAND2EQ:
		if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
			return 1;
		codec_drv->band2_narrow_wide = (adc_dac_narrow_wide & 0x01) << 8;
		codec_drv->band2_frequency = center_frequency << 5;
		codec_drv->band2_gain = gain;
		break;
	case	NAU88C22_INTOP_SET_BAND3EQ:
		if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
			return 1;
		codec_drv->band3_narrow_wide = (adc_dac_narrow_wide & 0x01) << 8;
		codec_drv->band3_frequency = center_frequency << 5;
		codec_drv->band3_gain = gain;
		break;
	case	NAU88C22_INTOP_SET_BAND4EQ:
		if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
			return 1;
		codec_drv->band4_narrow_wide = (adc_dac_narrow_wide & 0x01) << 8;
		codec_drv->band4_frequency = center_frequency << 5;
		codec_drv->band4_gain = gain;
		break;
	case	NAU88C22_INTOP_SET_BAND5EQ:
		if ( ( band > 4 ) || ( gain > 0x18 ) || ( center_frequency > 3 ))
			return 1;
		codec_drv->band5_frequency = center_frequency << 5;
		codec_drv->band5_gain = gain;
		break;
	case	NAU88C22_INTOP_WRITE_EQ:
		Nau88c22_WriteReg(bus , device_address,NAU88C22_EQ1,  codec_drv->adc_dac_path |      codec_drv->band1_frequency | codec_drv->band1_gain);
		Nau88c22_WriteReg(bus , device_address,NAU88C22_EQ2,  codec_drv->band2_narrow_wide | codec_drv->band2_frequency | codec_drv->band2_gain);
		Nau88c22_WriteReg(bus , device_address,NAU88C22_EQ3,  codec_drv->band3_narrow_wide | codec_drv->band3_frequency | codec_drv->band3_gain);
		Nau88c22_WriteReg(bus , device_address,NAU88C22_EQ4,  codec_drv->band4_narrow_wide | codec_drv->band4_frequency | codec_drv->band4_gain);
		Nau88c22_WriteReg(bus , device_address,NAU88C22_EQ5,                                codec_drv->band5_frequency | codec_drv->band5_gain);
		break;
	case	NAU88C22_INTOP_SET_VOLUME:
		/* here adc_dac_narrow_wide used as volume */
		uint16_t		reg = 0x100 | (adc_dac_narrow_wide & 0x3f);
		codec_drv->master_volume = reg;
		Nau88c22_WriteReg(bus , device_address,NAU88C22_LOUT1_HP_CONTROL,  reg);
		Nau88c22_WriteReg(bus , device_address,NAU88C22_ROUT1_HP_CONTROL, reg);
		break;
	case	NAU88C22_INTOP_READ_REG:
		/* here adc_dac_narrow_wide used as read reg index */
		return codec_drv->shadowregs[adc_dac_narrow_wide];
		break;
	}
	return 0;
}

ITCM_AREA_CODE uint32_t	nau88c22_codec_register(Nau88C22_Drv_TypeDef *private_data)
{
Nau88C22_Drv_TypeDef	*codec_drv;
	if ( ANALOG_DriverStruct[last_analog_used_handle].process == 0 )
	{
		ANALOG_DriverStruct[last_analog_used_handle].process = get_current_process();
		ANALOG_DriverStruct[last_analog_used_handle].private_data = (uint32_t *)private_data;

		codec_drv = (Nau88C22_Drv_TypeDef *)ANALOG_DriverStruct[last_analog_used_handle].private_data;
		if ( codec_drv->bus == NULL)
			return DRIVER_REQUEST_FAILED;
		ANALOG_DriverStruct[last_analog_used_handle].status = DRIVER_STATUS_IN_USE;
		ANALOG_DriverStruct[last_analog_used_handle].codec_start = nau88c22_start;
		ANALOG_DriverStruct[last_analog_used_handle].codec_stop = nau88c22_stop;
		ANALOG_DriverStruct[last_analog_used_handle].codec_get_status = nau88c22_get_status;
		ANALOG_DriverStruct[last_analog_used_handle].codec_init = nau88c22_init;
		ANALOG_DriverStruct[last_analog_used_handle].codec_internal_ops = nau88c22_internal_ops;

		last_analog_used_handle++;
		analog_driver_request++;
		return last_analog_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}
#endif // #ifdef A_OS_I2C_ENABLED
