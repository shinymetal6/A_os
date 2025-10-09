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
 * spi.c
 *
 *  Created on: Nov 23, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED

#include "spi.h"
#include <string.h>

SYSTEM_RAM	SPI_DriverStruct_t	SPI_DriverStruct[MAX_SPI_DEVICES];
SYSTEM_RAM	uint8_t				last_spi_used_handle,spi_driver_request;


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
uint32_t	i;
	for(i=0;i<MAX_SPI_DEVICES;i++)
	{
		if ( SPI_DriverStruct[i].bus != NULL )
		{
			if ( SPI_DriverStruct[i].bus  == hspi )
			{
#ifdef LCD_ENABLED
				SPI_LCD_DriverStruct_t	*spi_lcd_Drv = (SPI_LCD_DriverStruct_t *)SPI_DriverStruct[i].driver_private_data;
				spi_lcd_Drv->flags  |= SPI_DMA_DONE;
#endif // #ifdef LCD_ENABLED
			}
		}
	}
}
#endif // #ifdef A_OS_SPI_ENABLED
