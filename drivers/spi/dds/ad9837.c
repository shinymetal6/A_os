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

// Internal helper to toggle FSYNC and write 16 bits (MSB first)
static void AD9837_Write16(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv, uint16_t data)
{
    uint8_t tx_buf[2];
    tx_buf[0] = (data >> 8) & 0xFF; // MSB
    tx_buf[1] = data & 0xFF;        // LSB

    // FSYNC must be pulled LOW before transmission
    HAL_GPIO_WritePin(spi_ad9837_Drv->cs_port, spi_ad9837_Drv->cs_bit, GPIO_PIN_RESET);

    // Send 16 bits via SPI
    HAL_SPI_Transmit(spi_ad9837_Drv->bus, tx_buf, 2, spi_ad9837_Drv->spi_timeout_ms);

    // FSYNC must be pulled HIGH after transmission
    HAL_GPIO_WritePin(spi_ad9837_Drv->cs_port, spi_ad9837_Drv->cs_bit, GPIO_PIN_SET);
}

void AD9837_SetFrequency(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv, uint32_t freq_hz)
{
    // Formula: FREQ_REG = (F_OUT * 2^28) / F_MCLK
    // 2^28 = 268435456
	spi_ad9837_Drv->frequency = freq_hz;
    uint64_t freq_reg = ((uint64_t)spi_ad9837_Drv->frequency * 268435456ULL) / spi_ad9837_Drv->mclk_freq;

    // Split into two 14-bit writes (LSB first, then MSB)
    // 0x4000 is the command prefix for Frequency Register 0 (01xxxxxx xxxxxxxx)
    uint16_t lsb = 0x4000 | (freq_reg & 0x3FFF);
    uint16_t msb = 0x4000 | ((freq_reg >> 14) & 0x3FFF);

    AD9837_Write16(spi_ad9837_Drv, lsb);
    AD9837_Write16(spi_ad9837_Drv, msb);
}

void AD9837_SetWaveform(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv, AD9837_Waveform_t waveform)
{
    AD9837_Write16(spi_ad9837_Drv, (uint16_t)waveform);
}

void AD9837_Reset(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv)
{
    AD9837_Write16(spi_ad9837_Drv, AD9837_CMD_RESET);
}

void AD9837_Sleep(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv, uint8_t enable)
{
    uint16_t cmd = AD9837_CMD_B28; // Keep B28=1 for normal operation
    if (enable)
        cmd |= (AD9837_CMD_SLEEP1 | AD9837_CMD_SLEEP12);
    AD9837_Write16(spi_ad9837_Drv, cmd);
}

void AD9837_Init(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv)
{

    // Ensure FSYNC starts HIGH
    HAL_GPIO_WritePin(spi_ad9837_Drv->cs_port, spi_ad9837_Drv->cs_bit, GPIO_PIN_SET);

    // Reset the device to a known state
    AD9837_Reset(spi_ad9837_Drv);

    // Small delay to allow internal reset to complete
    HAL_Delay(1);

    // Clear reset and set default to Sine wave, awake
    AD9837_SetWaveform(spi_ad9837_Drv, AD9837_WAVE_SINE);
    AD9837_SetFrequency(spi_ad9837_Drv, AD9837_WAVE_SINE);
}

ITCM_AREA_CODE uint32_t	AD9837_register(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv)
{
SPI_NRF24L01_DriverStruct_t *eptr;
/*
 * CPOL = HIGH
 * CPHA = 1 edge
 * 16 bit MSB first
 */

	if ( spi_ad9837_Drv->cs_port == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_ad9837_Drv->bus == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_ad9837_Drv->mclk_freq == 0 )
		spi_ad9837_Drv->mclk_freq = 4000000;

	if ( spi_drv_ptr == NULL)
	{
		spi_drv_ptr = (SPI_DriverStruct_t *)spi_ad9837_Drv;
		spi_ad9837_Drv->next_drv = NULL;
	}
	else
	{
		eptr = (SPI_NRF24L01_DriverStruct_t *)spi_drv_ptr;
		while(eptr->next_drv != NULL)
			eptr = (SPI_NRF24L01_DriverStruct_t *)eptr->next_drv;
		eptr->next_drv = (uint32_t *)spi_ad9837_Drv;
		spi_ad9837_Drv->next_drv = NULL;
	}

	spi_ad9837_Drv->process = get_current_process();
	if( spi_ad9837_Drv->spi_timeout_ms == 0 )
		spi_ad9837_Drv->spi_timeout_ms = AD9837_SPI_TIMEOUT;
	if( spi_ad9837_Drv->waveform == 0 )
		spi_ad9837_Drv->waveform = AD9837_WAVE_SINE;

	AD9837_Init(spi_ad9837_Drv);
	return 0;
}

#endif // #ifdef A_OS_SPI_ENABLED
