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
 * spi_lcd.c
 *
 *  Created on: Nov 23, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../../kernel/system_default.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"
#include "../../../kernel/scheduler.h"
//#include "../../../kernel/kernel_opt.h"

#ifdef STM32H7xx_HAL_SPI_H

#include "spi_lcd.h"
#include <string.h>

extern	SPI_DriverStruct_t	SPI_DriverStruct[MAX_SPI_DEVICES];
extern	uint8_t				last_spi_used_handle,spi_driver_request;

ITCM_AREA_CODE uint32_t	spi_lcd_fill_rect(uint8_t handle,uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
SPI_LCD_DriverStruct_t	*spi_lcd_Drv = (SPI_LCD_DriverStruct_t *)SPI_DriverStruct[handle].driver_private_data;
	spi_lcd_Drv->lcd_fill_rect(x, y, w, h, color);
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_clear_screen(uint8_t handle,uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
SPI_LCD_DriverStruct_t	*spi_lcd_Drv = (SPI_LCD_DriverStruct_t *)SPI_DriverStruct[handle].driver_private_data;
	spi_lcd_Drv->lcd_clear_screen();
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_init(uint8_t handle)
{
SPI_LCD_DriverStruct_t	*spi_lcd_Drv = (SPI_LCD_DriverStruct_t *)SPI_DriverStruct[handle].driver_private_data;
	spi_lcd_Drv->lcd_init();
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_reset(uint8_t handle)
{
SPI_LCD_DriverStruct_t	*spi_lcd_Drv = (SPI_LCD_DriverStruct_t *)SPI_DriverStruct[handle].driver_private_data;
	spi_lcd_Drv->lcd_reset();
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_set_brightness(uint8_t handle,uint16_t brightness)
{
SPI_LCD_DriverStruct_t	*spi_lcd_Drv = (SPI_LCD_DriverStruct_t *)SPI_DriverStruct[handle].driver_private_data;
	if ( brightness <= FULL_BRIGHTNESS)
	{
		spi_lcd_Drv->backlight_timer->Instance->CCR1 = brightness;
		spi_lcd_Drv->current_brightness = brightness;
	}
	return 0;
}


ITCM_AREA_CODE uint32_t	spi_lcd_register(I2C_24xx_Drv_TypeDef *driver_private_data)
{
SPI_LCD_DriverStruct_t	*spi_lcd_Drv;
	if ( SPI_DriverStruct[last_spi_used_handle].process == 0 )
	{
		if ( driver_private_data->wakeup_id == 0 )
			return DRIVER_REQUEST_FAILED;
		SPI_DriverStruct[last_spi_used_handle].process = get_current_process();
		SPI_DriverStruct[last_spi_used_handle].driver_private_data = (uint32_t *)driver_private_data;

		spi_lcd_Drv = (SPI_LCD_DriverStruct_t *)SPI_DriverStruct[last_spi_used_handle].driver_private_data;

		switch(spi_lcd_Drv->lcd_model)
		{
		case LCD_IS_7735 :
			ST7735_cs_port = spi_lcd_Drv->cs_port;
			ST7735_cs_bit = spi_lcd_Drv->cs_bit;
			ST7735_reset_port = spi_lcd_Drv->cs_port;
			ST7735_reset_bit = spi_lcd_Drv->cs_bit;
			ST7735_dc_port = spi_lcd_Drv->cs_port;
			ST7735_dc_bit = spi_lcd_Drv->cs_bit;
			if ( spi_lcd_Drv->reset_time == 0 )
				spi_lcd_Drv->reset_time = DEFAULT_RESET_TIME;
			ST7735_reset_time = spi_lcd_Drv->reset_time;
			spi_lcd_Drv->lcd_reset = ST7735_Reset;
			spi_lcd_Drv->lcd_init = ST7735_Init;
			spi_lcd_Drv->lcd_fill_rect = ST7735_FillRectangle;
			spi_lcd_Drv->lcd_clear_screen = ST7735_ClearScreen;
			break;
		case LCD_IS_9341 :
			break;
		default :
			return DRIVER_REQUEST_FAILED;
		}

		if ( spi_lcd_Drv->bus == NULL )
			return DRIVER_REQUEST_FAILED;
		if ( spi_lcd_Drv->backlight_timer == NULL )
			return DRIVER_REQUEST_FAILED;
		if ( spi_lcd_Drv->cs_port == NULL )
			return DRIVER_REQUEST_FAILED;
		if ( spi_lcd_Drv->reset_port == NULL )
			return DRIVER_REQUEST_FAILED;
		if ( spi_lcd_Drv->dc_port == NULL )
			return DRIVER_REQUEST_FAILED;

		SPI_DriverStruct[last_spi_used_handle].status = DRIVER_STATUS_IN_USE;
		last_spi_used_handle++;
		return last_spi_used_handle-1;
	}
	return DRIVER_REQUEST_FAILED;
}

#endif // #ifdef STM32H7xx_HAL_SPI_H

