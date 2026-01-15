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
 * spi.h
 *
 *  Created on: Nov 23, 2024
 *      Author: fil
 */

#ifndef DRIVERS_SPI_SPI_LCD_H_
#define DRIVERS_SPI_SPI_LCD_H_

#ifdef A_OS_SPI_ENABLED
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
}SPI_DriverStruct_t;

/* flags */
#define	SPI_USES_DMA		0x80
#define	SPI_FLAGS_WAKEUP	0x40
#define	SPI_DMA_DONE		0x04
#define	SPI_TX_COMPLETE		0x02
#define	SPI_RX_COMPLETE		0x01

#include "lcd/lcd.h"
#include "NFC/nfc.h"
#include "nrf24l01/nrf24l01.h"

extern	SPI_DriverStruct_t 	*spi_drv_ptr;

#endif // #ifdef A_OS_SPI_ENABLED

#endif /* DRIVERS_SPI_LCD_SPI_LCD_H_ */
