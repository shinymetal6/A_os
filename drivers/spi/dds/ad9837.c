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
 * ad9837.c
 *
 *  Created on: Mar 19, 2026
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#include "ad9837.h"

#define FSYNC_LOW()   HAL_GPIO_WritePin(AD9837_FSYNC_PORT, AD9837_FSYNC_PIN, GPIO_PIN_RESET)


ITCM_AREA_CODE uint32_t spi_ad9837_WriteReg(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint16_t data)
{
	uint32_t ret_val;
	HAL_GPIO_WritePin(spi_ad9837_Drv->cs_port, spi_ad9837_Drv->cs_bit, GPIO_PIN_RESET);
    // SPI must be configured for 16-bit data size in CubeMX
	ret_val = HAL_SPI_Transmit(spi_ad9837_Drv->bus, (uint8_t*)&data, 1, 100);
	HAL_GPIO_WritePin(spi_ad9837_Drv->cs_port, spi_ad9837_Drv->cs_bit, GPIO_PIN_SET);
	return ret_val;
}

ITCM_AREA_CODE static void	spi_ad9837_init(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv)
{
	HAL_GPIO_WritePin(spi_ad9837_Drv->cs_port, spi_ad9837_Drv->cs_bit, GPIO_PIN_SET);
    // Reset the device
	spi_ad9837_WriteReg(spi_ad9837_Drv,AD9837_CMD_RESET);
    // Clear reset, enable DAC, default to Sine
    // B28=1 (28-bit freq), HLB=0 (Freq0), Mode=Sine
	spi_ad9837_WriteReg(spi_ad9837_Drv,AD9837_CMD_B28 | spi_ad9837_Drv->waveform);
}

ITCM_AREA_CODE void spi_ad9837_SetFrequency(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint32_t freq_hz)
{
uint64_t tuning_word;
uint16_t freq_msb;
uint16_t freq_lsb;

	spi_ad9837_Drv->frequency = freq_hz;
    // Calculate Tuning Word: FTW = (Fout * 2^28) / MCLK
    // Use 64-bit integer to prevent overflow
    tuning_word = ((uint64_t)freq_hz * (1ULL << 28)) / AD9837_MCLK_FREQ;

    // Split into two 14-bit words
    // MSB contains bits 27-14
    // LSB contains bits 13-0
    freq_msb = (tuning_word >> 14) & 0x3FFF;
    freq_lsb = tuning_word & 0x3FFF;

    // Add Command Bits to MSB and LSB
    // B28=1, HLB=0 (Select Freq0), D15/D14 = 01 for Freq Reg
    freq_msb |= 0x4000; // 01xxxxxx... (Freq0 MSB)
    freq_lsb |= 0x4000; // 01xxxxxx... (Freq0 LSB)

    // Note: The AD9837 requires the Control Register to have B28=1
    // before sending the two frequency words. This was done in Init.
    // If you changed it, you must send Control Reg (0x2000) first.

    // Write MSB then LSB
    spi_ad9837_WriteReg(spi_ad9837_Drv,freq_msb);
    spi_ad9837_WriteReg(spi_ad9837_Drv,freq_lsb);
}

/**
 * @brief Set Output Phase Offset
 * @param phase_deg: Phase in degrees (0 to 360)
 */
ITCM_AREA_CODE void spi_ad9837_SetPhase(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint16_t phase_deg)
{
    uint16_t phase_reg;

	spi_ad9837_Drv->phase_deg = phase_deg;
	// 12-bit phase resolution (4096 steps)
    // Phase = (Degrees / 360) * 4096
    phase_reg = ((uint32_t)phase_deg * 4096) / 360;
    phase_reg &= 0x0FFF; // Mask to 12 bits

    // Command: 0x4000 (Phase Reg 0)
    phase_reg |= 0x4000;

    spi_ad9837_WriteReg(spi_ad9837_Drv,phase_reg);
}

/**
 * @brief Select Waveform Mode
 * @param mode: AD9837_MODE_SINE, _TRIANGLE, or _SQUARE
 */
ITCM_AREA_CODE void spi_ad9837_SetWaveform(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint8_t waveform)
{
    // Read current control state (simplified: we just overwrite known bits)
    // We keep B28=1 (0x2000) and HLB=0 (0x0000)
	if ((waveform != AD9837_MODE_SINE) && (waveform != AD9837_MODE_TRIANGLE) && (waveform != AD9837_MODE_SQUARE))
		return;
	spi_ad9837_Drv->waveform = waveform;
	uint16_t ctrl_reg = AD9837_CMD_B28 | waveform;
    spi_ad9837_WriteReg(spi_ad9837_Drv,ctrl_reg);
}

/**
 * @brief Put device to sleep to save power
 * @param enable: 1 to sleep, 0 to wake
 */
ITCM_AREA_CODE void spi_ad9837_Sleep(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint8_t enable)
{
    uint16_t ctrl_reg = AD9837_CMD_B28 | AD9837_MODE_SINE;
    if(enable) {
        ctrl_reg |= AD9837_SLEEP12; // Sleep DAC and Core
    }
    spi_ad9837_WriteReg(spi_ad9837_Drv,ctrl_reg);
}

ITCM_AREA_CODE uint32_t	spi_ad9837_register(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv)
{
SPI_NRF24L01_DriverStruct_t *eptr, *pre_eptr;

	if ( spi_ad9837_Drv->cs_port == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_ad9837_Drv->bus == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_drv_ptr == NULL)
	{
		spi_drv_ptr = (SPI_DriverStruct_t *)spi_ad9837_Drv;
		spi_ad9837_Drv->next_drv = NULL;
	}
	else
	{
		eptr = pre_eptr = (SPI_NRF24L01_DriverStruct_t *)spi_drv_ptr;
		while(eptr->next_drv != NULL)
		{
			pre_eptr = eptr;
			eptr = (SPI_NRF24L01_DriverStruct_t *)eptr->next_drv;
		}
		pre_eptr->next_drv = (uint32_t *)spi_ad9837_Drv;
		spi_ad9837_Drv->next_drv = NULL;
	}

	spi_ad9837_Drv->process = get_current_process();
	if( spi_ad9837_Drv->spi_timeout_ms == 0 )
		spi_ad9837_Drv->spi_timeout_ms = AD9837_SPI_TIMEOUT;
	if( spi_ad9837_Drv->waveform == 0 )
		spi_ad9837_Drv->waveform = AD9837_MODE_SINE;

	spi_ad9837_init(spi_ad9837_Drv);
	return 0;
}

#endif // #ifdef A_OS_SPI_ENABLED
