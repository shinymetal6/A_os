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
 * wm8994.c
 *
 *  Created on: Feb 26, 2024
 *      Author: fil
 */
//  I2Cx_WriteMultiple(&hI2cAudioHandler, Addr, Reg, I2C_MEMADD_SIZE_16BIT,(uint8_t*)&Value, 2);
#include "main.h"
#include "../../kernel/system_default.h"
extern	void task_delay(uint32_t tick_count);

#ifdef CODEC_WM8994
#include "wm8994.h"

void wm8994_IO_Write(uint16_t Reg, uint16_t Value)
{
uint16_t tmp = Value;

	Value = ((uint16_t)(tmp >> 8) & 0x00FF);
	Value |= ((uint16_t)(tmp << 8)& 0xFF00);
	HAL_I2C_Mem_Write(&CODEC_WM8994_I2C, CODEC_WM8994_I2C_ADDRESS, Reg,I2C_MEMADD_SIZE_16BIT,(uint8_t*)&Value, 2,CODEC_WM8994_TIMEOUT);
}

uint16_t wm8994_IO_Read(uint16_t Reg)
{
uint16_t read_value = 0, tmp = 0;

	HAL_I2C_Mem_Read(&CODEC_WM8994_I2C, CODEC_WM8994_I2C_ADDRESS, Reg,I2C_MEMADD_SIZE_16BIT,(uint8_t*)&read_value, 2,CODEC_WM8994_TIMEOUT);
	tmp = ((uint16_t)(read_value >> 8) & 0x00FF);
	tmp |= ((uint16_t)(read_value << 8)& 0xFF00);
	read_value = tmp;
	return read_value;
}


void wm8994_Init(uint32_t AudioFreq)
{
	uint16_t power_mgnt_reg_1 = 0;

  /* wm8994 Errata Work-Arounds */
	wm8994_IO_Write(0x102, 0x0003);
	wm8994_IO_Write(0x817, 0x0000);
	wm8994_IO_Write(0x102, 0x0000);
	/* Enable VMID soft start (fast), Start-up Bias Current Enabled */
	wm8994_IO_Write(0x39, 0x006C);
	wm8994_IO_Write(0x01, 0x0013);

/* OUTPUT_DEVICE_HEADPHONE: */

	wm8994_IO_Write(0x05, 0x0303);		/* Disable DAC1 (Left), Disable DAC1 (Right),	Enable DAC2 (Left), Enable DAC2 (Right)*/
	wm8994_IO_Write(0x601, 0x0001);		/* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
	wm8994_IO_Write(0x602, 0x0001);		/* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
	wm8994_IO_Write(0x604, 0x0000);		/* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
	wm8994_IO_Write(0x605, 0x0000);		/* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */

/* INPUT_DEVICE_INPUT_LINE_1 */
	wm8994_IO_Write(0x28, 0x0011);		/* IN1LN_TO_IN1L, IN1LP_TO_VMID, IN1RN_TO_IN1R, IN1RP_TO_VMID */
	wm8994_IO_Write(0x29, 0x0035);		/* Disable mute on IN1L_TO_MIXINL and +30dB on IN1L PGA output */
	wm8994_IO_Write(0x2A, 0x0035);		/* Disable mute on IN1R_TO_MIXINL, Gain = +30dB */
	wm8994_IO_Write(0x04, 0x0303);		/* Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right) Enable Left ADC, Enable Right ADC */
	wm8994_IO_Write(0x440, 0x00DB);		/* Enable AIF1 DRC1 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0 */
	wm8994_IO_Write(0x02, 0x6350);		/* Enable IN1L and IN1R, Disable IN2L and IN2R, Enable Thermal sensor & shutdown */
	wm8994_IO_Write(0x606, 0x0002);		/* Enable the ADCL(Left) to AIF1 Timeslot 0 (Left) mixer path */
	wm8994_IO_Write(0x607, 0x0002);		/* Enable the ADCR(Right) to AIF1 Timeslot 0 (Right) mixer path */
	wm8994_IO_Write(0x700, 0x000D);		/* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect */

/* AUDIO_FREQUENCY_48K */
	wm8994_IO_Write(0x210, 0x0083);		/* AIF1 Sample Rate = 48 (KHz), ratio=256 */
    wm8994_IO_Write(0x2D, 0x0100);		/* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
    wm8994_IO_Write(0x2E, 0x0100);		/* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
/* Startup sequence for Headphone */
	wm8994_IO_Write(0x110,0x8100);
	task_delay(300);

    /* Soft un-Mute the AIF1 Timeslot 0 DAC1 path L&R */
    wm8994_IO_Write(0x420, 0x0000);
	wm8994_IO_Write(0x51, 0x0005);

	/* Enable bias generator, Enable VMID, Enable HPOUT1 (Left) and Enable HPOUT1 (Right) input stages */
	/* idem for Speaker */
	power_mgnt_reg_1 |= 0x0303 | 0x3003;
	wm8994_IO_Write(0x01, power_mgnt_reg_1);

	/* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate stages */
	wm8994_IO_Write(0x60, 0x0022);

	/* Enable Charge Pump */
	wm8994_IO_Write(0x4C, 0x9F25);

	/* Add Delay */
	task_delay(15);

	/* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
	wm8994_IO_Write(0x2D, 0x0001);

	/* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
	wm8994_IO_Write(0x2E, 0x0001);

	/* Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR) */
	/* idem for SPKOUTL and SPKOUTR */
	wm8994_IO_Write(0x03, 0x0030 | 0x0300);

	/* Enable DC Servo and trigger start-up mode on left and right channels */
	wm8994_IO_Write(0x54, 0x0033);

	/* Add Delay */
	task_delay(257);

	/* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate and output stages. Remove clamps */
	wm8994_IO_Write(0x60, 0x00EE);

	/* Unmutes */

	/* Unmute DAC 1 (Left) */
	wm8994_IO_Write(0x610, 0x00C0);

	/* Unmute DAC 1 (Right) */
	wm8994_IO_Write(0x611, 0x00C0);

	/* Unmute the AIF1 Timeslot 0 DAC path */
	wm8994_IO_Write(0x420, 0x0010);

	/* Unmute DAC 2 (Left) */
	wm8994_IO_Write(0x612, 0x00C0);

	/* Unmute DAC 2 (Right) */
	wm8994_IO_Write(0x613, 0x00C0);

	/* Unmute the AIF1 Timeslot 1 DAC2 path */
	wm8994_IO_Write(0x422, 0x0010);

}

uint32_t wm8994_ReadID(void)
{
	wm8994_Init(SAMPLE_FREQUENCY);
	return ((uint32_t)wm8994_IO_Read(WM8994_CHIPID_ADDR));
}

uint32_t wm8994_Reset(void)
{
	wm8994_IO_Write( 0x0000, 0x0000);
	return 0;
}

uint32_t wm8994_SetMute(uint32_t Cmd)
{
	/* Set the Mute mode */
	if(Cmd == AUDIO_MUTE_ON)
	{
		wm8994_IO_Write(0x420, 0x0200);	/* Soft Mute the AIF1 Timeslot 0 DAC1 path L&R */
		wm8994_IO_Write(0x422, 0x0200);	/* Soft Mute the AIF1 Timeslot 1 DAC2 path L&R */
	}
	else /* AUDIO_MUTE_OFF Disable the Mute */
	{
		wm8994_IO_Write(0x420, 0x0010);	/* Unmute the AIF1 Timeslot 0 DAC1 path L&R */
		wm8994_IO_Write(0x422, 0x0010);	/* Unmute the AIF1 Timeslot 1 DAC2 path L&R */
	}
	return 0;
}

uint32_t wm8994_SetVolume(uint8_t Volume)
{
uint8_t convertedvol = VOLUME_CONVERT(Volume);

	/* Output volume */
	if(convertedvol > 0x3E)
	{
		wm8994_SetMute(AUDIO_MUTE_OFF);					/* Unmute audio codec */
		wm8994_IO_Write(0x1C, 0x3F | 0x140);			/* Left Headphone Volume */
		wm8994_IO_Write(0x1D, 0x3F | 0x140);			/* Right Headphone Volume */
		wm8994_IO_Write(0x26, 0x3F | 0x140);			/* Left Speaker Volume */
		wm8994_IO_Write(0x27, 0x3F | 0x140);			/* Right Speaker Volume */
	}
	else if (Volume == 0)
	{
		wm8994_SetMute(AUDIO_MUTE_ON);					/* Mute audio codec */
	}
	else
	{
		wm8994_SetMute(AUDIO_MUTE_OFF);					/* Unmute audio codec */
		wm8994_IO_Write(0x1C, convertedvol | 0x140);	/* Left Headphone Volume */
		wm8994_IO_Write(0x1D, convertedvol | 0x140);	/* Right Headphone Volume */
		wm8994_IO_Write(0x26, convertedvol | 0x140);	/* Left Speaker Volume */
		wm8994_IO_Write(0x27, convertedvol | 0x140);	/* Right Speaker Volume */
	}

	/* Input volume */
	convertedvol = VOLUME_IN_CONVERT(Volume);
	wm8994_IO_Write(0x400, convertedvol | 0x100);		/* Left AIF1 ADC1 volume */
	wm8994_IO_Write(0x401, convertedvol | 0x100);		/* Right AIF1 ADC1 volume */
	wm8994_IO_Write(0x404, convertedvol | 0x100);		/* Left AIF1 ADC2 volume */
	wm8994_IO_Write(0x405, convertedvol | 0x100);		/* Right AIF1 ADC2 volume */
	return 0;
}
#endif // #ifdef CODEC_WM8994


