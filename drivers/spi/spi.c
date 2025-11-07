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

SPI_DriverStruct_t 	*spi_drv_ptr;

ITCM_AREA_CODE static void spi_irq_common(SPI_HandleTypeDef *hspi,uint32_t flag)
{
SPI_DriverStruct_t	*spi_drv_ptr_L = spi_drv_ptr;
	if ( spi_drv_ptr == NULL )
		return;
	while(spi_drv_ptr_L->bus != hspi)
	{
		if ( spi_drv_ptr_L->next_drv != NULL )
			spi_drv_ptr_L = (SPI_DriverStruct_t *)i2c_drv_ptr->next_drv;
	}
	if (spi_drv_ptr_L != NULL)
	{
		if ( spi_drv_ptr_L->process != 0 )
		{
			spi_drv_ptr_L->flags |= flag;
		}
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	spi_irq_common(hspi,SPI_TX_COMPLETE | SPI_DMA_DONE);
	/*

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
	*/
}
#endif // #ifdef A_OS_SPI_ENABLED
