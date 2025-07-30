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
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	SPI_HandleTypeDef 	*bus;
	uint32_t			*driver_private_data;
}SPI_DriverStruct_t;

/* flags */
#define	SPI_TX_DMA_COMPLETE	0x80

#include "spi_lcd/spi_lcd.h"
#include "LoRa/LoRa.h"

#endif // #ifdef A_OS_SPI_ENABLED

#endif /* DRIVERS_SPI_LCD_SPI_LCD_H_ */
