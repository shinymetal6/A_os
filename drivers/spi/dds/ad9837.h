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
 * ad9837.h
 *
 *  Created on: Mar 19, 2026
 *      Author: fil
 */

#ifndef DRIVERS_SPI_DDS_AD9837_H_
#define DRIVERS_SPI_DDS_AD9837_H_

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	SPI_HandleTypeDef 	*bus;
	GPIO_TypeDef	 	*cs_port;
	uint16_t			cs_bit;
	uint32_t 			wakeup_id;
	uint32_t			*next_drv;
	/* driver proprietary data */
	uint32_t 			spi_timeout_ms;
	uint8_t 			waveform;
	uint16_t 			phase_deg;
	uint32_t 			frequency;
}SPI_AD9837_DriverStruct_t;
#define	AD9837_SPI_TIMEOUT					100

// Master Clock Frequency (Hz) - CHANGE THIS TO MATCH YOUR OSCILLATOR
#define AD9837_MCLK_FREQ        4000000UL

/* --- Register Definitions --- */
#define AD9837_CMD_RESET        0x0100
#define AD9837_CMD_HLB          0x0000 // Write to FREQ0/PHASE0
#define AD9837_CMD_B28          0x2000 // 28-bit Frequency Write
#define AD9837_MODE_SINE        0x0000
#define AD9837_MODE_TRIANGLE    0x0002
#define AD9837_MODE_SQUARE      0x0008
#define AD9837_SLEEP1           0x0040
#define AD9837_SLEEP12          0x0080

extern uint32_t	spi_ad9837_register(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv);
extern void spi_ad9837_Sleep(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint8_t enable);
extern void spi_ad9837_SetWaveform(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint8_t mode);
extern void spi_ad9837_SetPhase(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint16_t phase_deg);
extern void spi_ad9837_SetFrequency(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv,uint32_t freq_hz);

#endif /* DRIVERS_SPI_DDS_AD9837_H_ */
