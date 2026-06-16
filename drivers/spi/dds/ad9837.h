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



#define	AD9837_SPI_TIMEOUT					100

// AD9837 Control Register Bit Definitions
#define AD9837_CMD_B28      (1 << 13) // 0x2000: Write 28-bit frequency in two 14-bit writes
#define AD9837_CMD_HLB      (1 << 12) // 0x1000: Write 14-bit LSB (ignored if B28=1)
#define AD9837_CMD_FSELECT  (1 << 11) // 0x0800: Select Frequency Register 1
#define AD9837_CMD_PSELECT  (1 << 10) // 0x0400: Select Phase Register 1
#define AD9837_CMD_RESET    (1 << 9)  // 0x0200: Reset internal registers
#define AD9837_CMD_SLEEP1   (1 << 8)  // 0x0100: Power down internal clock
#define AD9837_CMD_SLEEP12  (1 << 7)  // 0x0080: Power down internal DAC
#define AD9837_CMD_OPBITEN  (1 << 6)  // 0x0040: Enable square wave output
#define AD9837_CMD_DIV2     (1 << 4)  // 0x0010: Divide MSB by 2 (for square wave)
#define AD9837_CMD_MODE     (1 << 3)  // 0x0008: 0 = Sine, 1 = Triangle/Square

// Waveform Selection Enum
typedef enum {
    AD9837_WAVE_SINE = (AD9837_CMD_B28),
    AD9837_WAVE_TRIANGLE = (AD9837_CMD_B28 | AD9837_CMD_MODE),
    AD9837_WAVE_SQUARE = (AD9837_CMD_B28 | AD9837_CMD_OPBITEN | AD9837_CMD_DIV2 | AD9837_CMD_MODE)
} AD9837_Waveform_t;


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
	AD9837_Waveform_t	waveform;
	uint16_t 			phase_deg;
	uint32_t 			mclk_freq;
	uint32_t 			frequency;
}SPI_AD9837_DriverStruct_t;

// Function Prototypes
extern	uint32_t	AD9837_register(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv);
extern	void 		AD9837_Init(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv);
extern	void 		AD9837_SetFrequency(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv, uint32_t freq_hz);
extern	void 		AD9837_SetWaveform(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv, AD9837_Waveform_t waveform);
extern	void 		AD9837_Reset(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv);
extern	void 		AD9837_Sleep(SPI_AD9837_DriverStruct_t *spi_ad9837_Drv, uint8_t enable);

#endif /* DRIVERS_SPI_DDS_AD9837_H_ */
