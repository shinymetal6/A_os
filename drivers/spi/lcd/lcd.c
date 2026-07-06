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
 * lcd.c
 *
 *  Created on: Nov 7, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../../kernel/A.h"
#include "../../../kernel/A_exported_functions.h"

#ifdef A_OS_SPI_ENABLED
#ifdef LCD_ENABLED

#include "lcd.h"

__attribute__ ((aligned (32)))	uint16_t	framebuffer_rect[MAX_WIDTH*MAX_HEIGHT];

ITCM_AREA_CODE uint32_t	spi_lcd_fill_rect(SPI_LCD_DriverStruct_t *spi_lcd_Drv,uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	if ( spi_lcd_Drv->lcd_fill_rect != NULL )
		spi_lcd_Drv->lcd_fill_rect(x, y, w, h, color);
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_fill_screen(SPI_LCD_DriverStruct_t *spi_lcd_Drv, uint16_t color)
{
	if ( spi_lcd_Drv->lcd_fill_screen != NULL )
		spi_lcd_Drv->lcd_fill_screen(color);
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_write_string(SPI_LCD_DriverStruct_t *spi_lcd_Drv,uint16_t x, uint16_t y, char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	spi_lcd_Drv->time_start = DWT->CYCCNT;
	if ( spi_lcd_Drv->lcd_write_string != NULL )
		spi_lcd_Drv->lcd_write_string(x, y, str,font,color,bgcolor);
	spi_lcd_Drv->op_time = (DWT->CYCCNT - spi_lcd_Drv->time_start) / (HSI_CLOCK / 1000000);
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_clear_screen(SPI_LCD_DriverStruct_t *spi_lcd_Drv)
{
	spi_lcd_Drv->time_start = DWT->CYCCNT;
	if ( spi_lcd_Drv->lcd_clear_screen != NULL )
		spi_lcd_Drv->lcd_clear_screen();
	spi_lcd_Drv->op_time = (DWT->CYCCNT - spi_lcd_Drv->time_start) / (HSI_CLOCK / 1000000);
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_reset(SPI_LCD_DriverStruct_t *spi_lcd_Drv)
{
	if ( spi_lcd_Drv->lcd_reset != NULL )
		spi_lcd_Drv->lcd_reset();
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_draw_image(SPI_LCD_DriverStruct_t *spi_lcd_Drv, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* image)
{
	spi_lcd_Drv->time_start = DWT->CYCCNT;
	if ( spi_lcd_Drv->lcd_draw_image != NULL )
		spi_lcd_Drv->lcd_draw_image(x,y,w,h,image);
	spi_lcd_Drv->op_time = (DWT->CYCCNT - spi_lcd_Drv->time_start) / (HSI_CLOCK / 1000000);
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_set_brightness(SPI_LCD_DriverStruct_t *spi_lcd_Drv,uint16_t brightness)
{
#ifdef A_OS_TIMERS_ENABLED
	if ( brightness <= FULL_BRIGHTNESS)
	{
		switch(spi_lcd_Drv->backlight_timer_channel)
		{
		case	TIM_CHANNEL_1	:	spi_lcd_Drv->backlight_timer->Instance->CCR1 = brightness; break;
		case	TIM_CHANNEL_2	:	spi_lcd_Drv->backlight_timer->Instance->CCR2 = brightness; break;
		case	TIM_CHANNEL_3	:	spi_lcd_Drv->backlight_timer->Instance->CCR3 = brightness; break;
		case	TIM_CHANNEL_4	:	spi_lcd_Drv->backlight_timer->Instance->CCR4 = brightness; break;
		case	TIM_CHANNEL_5	:	spi_lcd_Drv->backlight_timer->Instance->CCR5 = brightness; break;
		case	TIM_CHANNEL_6	:	spi_lcd_Drv->backlight_timer->Instance->CCR6 = brightness; break;
		default : return 1;
		}
		HAL_TIM_PWM_Start(spi_lcd_Drv->backlight_timer,spi_lcd_Drv->backlight_timer_channel);
	}
#endif
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_init(SPI_LCD_DriverStruct_t *spi_lcd_Drv)
{
	if ( spi_lcd_Drv->lcd_init != NULL )
			spi_lcd_Drv->lcd_init();
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_on(SPI_LCD_DriverStruct_t *spi_lcd_Drv )
{
	spi_lcd_set_brightness(spi_lcd_Drv,FULL_BRIGHTNESS);
	return 0;
}


ITCM_AREA_CODE uint32_t	spi_lcd_off(SPI_LCD_DriverStruct_t *spi_lcd_Drv)
{
	spi_lcd_set_brightness(spi_lcd_Drv,ZERO_BRIGHTNESS);
	return 0;
}

ITCM_AREA_CODE uint32_t	spi_lcd_register(SPI_LCD_DriverStruct_t *spi_lcd_Drv)
{
SPI_LCD_DriverStruct_t *eptr;

	if ( spi_lcd_Drv->backlight_timer == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_lcd_Drv->cs_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_lcd_Drv->reset_port == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_lcd_Drv->dc_port == NULL )
		return DRIVER_REQUEST_FAILED;

	if ( spi_lcd_Drv->bus == NULL )
		return DRIVER_REQUEST_FAILED;
	if ( spi_drv_ptr == NULL)
	{
		spi_drv_ptr = (SPI_DriverStruct_t *)spi_lcd_Drv;
		spi_lcd_Drv->next_drv = NULL;
	}
	else
	{
		eptr = (SPI_LCD_DriverStruct_t *)spi_drv_ptr;
		while(eptr->next_drv != NULL)
			eptr = (SPI_LCD_DriverStruct_t *)eptr->next_drv;
		eptr->next_drv = (uint32_t *)spi_lcd_Drv;
		spi_lcd_Drv->next_drv = NULL;
	}

	spi_lcd_Drv->process = get_current_process();
	spi_lcd_Drv->bus = spi_lcd_Drv->bus;

	switch(spi_lcd_Drv->lcd_model)
	{
	case LCD_IS_7735 :
		ST7735_cs_port = spi_lcd_Drv->cs_port;
		ST7735_cs_bit = spi_lcd_Drv->cs_bit;
		ST7735_reset_port = spi_lcd_Drv->reset_port;
		ST7735_reset_bit = spi_lcd_Drv->reset_bit;
		ST7735_dc_port = spi_lcd_Drv->dc_port;
		ST7735_dc_bit = spi_lcd_Drv->dc_bit;
		ST7735_spi_port = spi_lcd_Drv->bus;
		if ( spi_lcd_Drv->reset_time == 0 )
			spi_lcd_Drv->reset_time = DEFAULT_RESET_TIME;
		ST7735_reset_time = spi_lcd_Drv->reset_time;
		spi_lcd_Drv->lcd_reset = ST7735_Reset;
		spi_lcd_Drv->lcd_init = ST7735_Init;
		spi_lcd_Drv->lcd_fill_rect = ST7735_FillRectangle;
		spi_lcd_Drv->lcd_fill_screen = ST7735_FillScreen;
		spi_lcd_Drv->lcd_clear_screen = ST7735_ClearScreen;
		spi_lcd_Drv->lcd_draw_image = ST7735_DrawImage;
		spi_lcd_Drv->lcd_invert_colors = ST7735_InvertColors;
		spi_lcd_Drv->lcd_write_string = ST7735_WriteString;
		spi_lcd_Drv->lcd_width = ST7735_WIDTH;
		spi_lcd_Drv->lcd_height = ST7735_HEIGHT;
		ST7735_flags = (uint8_t *)&spi_lcd_Drv->flags;
		spi_lcd_Drv->dma_timeout = SPI_LCD_DMA_TIMEOUT;
		ST7735_dma_timeout = (uint8_t *)&spi_lcd_Drv->dma_timeout;
		return 0;
		break;
	case LCD_IS_9341 :
		ILI9341_cs_port = spi_lcd_Drv->cs_port;
		ILI9341_cs_bit = spi_lcd_Drv->cs_bit;
		ILI9341_reset_port = spi_lcd_Drv->reset_port;
		ILI9341_reset_bit = spi_lcd_Drv->reset_bit;
		ILI9341_dc_port = spi_lcd_Drv->dc_port;
		ILI9341_dc_bit = spi_lcd_Drv->dc_bit;
		ILI9341_spi_port = spi_lcd_Drv->bus;
		if ( spi_lcd_Drv->reset_time == 0 )
			spi_lcd_Drv->reset_time = DEFAULT_RESET_TIME;
		ILI9341_reset_time = spi_lcd_Drv->reset_time;
		spi_lcd_Drv->lcd_reset = ILI9341_Reset;
		spi_lcd_Drv->lcd_init = ILI9341_Init;
		spi_lcd_Drv->lcd_fill_rect = ILI9341_FillRectangle;
		spi_lcd_Drv->lcd_clear_screen = ILI9341_ClearScreen;
		spi_lcd_Drv->lcd_draw_image = ILI9341_DrawImage;
		spi_lcd_Drv->lcd_invert_colors = ILI9341_InvertColors;
		spi_lcd_Drv->lcd_write_string = ILI9341_WriteString;
		spi_lcd_Drv->lcd_width = ILI9341_WIDTH;
		spi_lcd_Drv->lcd_height = ILI9341_HEIGHT;
		ILI9341_flags = (uint8_t *)&spi_lcd_Drv->flags;
		return 0;
		break;
	default :
		return DRIVER_REQUEST_FAILED;
	}
	return DRIVER_REQUEST_FAILED;
}

#endif // #ifdef LCD_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED



