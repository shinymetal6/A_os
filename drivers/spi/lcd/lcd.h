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
 * lcd.h
 *
 *  Created on: Nov 7, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SPI_LCD_LCD_H_
#define DRIVERS_SPI_LCD_LCD_H_

#include "../spi.h"
#include <string.h>
#include "fonts.h"

#define	LCD_IS_7735		0x7735
#define	LCD_IS_9341		0x9341

#define MAX_WIDTH  240
#define MAX_HEIGHT 320

#define	ZERO_BRIGHTNESS		0
#define	LOW_BRIGHTNESS		30
#define	HALF_BRIGHTNESS		500
#define	Hi_BRIGHTNESS		750
#define	FULL_BRIGHTNESS		1000

#define	DEFAULT_RESET_TIME	100
#define	SPI_LCD_DMA_TIMEOUT	250

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
#ifdef A_OS_TIMERS_ENABLED
	TIM_HandleTypeDef	*backlight_timer;
	uint32_t			backlight_timer_channel;
#endif
	GPIO_TypeDef	 	*reset_port;
	uint16_t			reset_bit;
	uint16_t			reset_time;
	uint8_t				active_level_reset_bit;
	GPIO_TypeDef	 	*dc_port;
	uint16_t			dc_bit;
	uint16_t			lcd_model;
	uint16_t			lcd_width;
	uint16_t			lcd_height;
	uint16_t			current_brightness;
	void				(*lcd_init)(void);
	void				(*lcd_reset)(void);
	void				(*lcd_clear_screen)	(void);
	void				(*lcd_fill_screen)	(uint16_t color);
	uint32_t			(*lcd_invert_colors)(uint8_t invert);
	uint32_t			(*lcd_write_char)	(uint16_t x, uint16_t y, char ch,    uint8_t font_index, uint16_t color, uint16_t bgcolor);
	uint32_t			(*lcd_write_string)	(uint16_t x, uint16_t y, char* str, FontDef font, uint16_t color, uint16_t bgcolor);
	uint32_t			(*lcd_fill_rect)  	(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	uint32_t			(*lcd_draw_image)  	(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* image);
	uint8_t				dma_timeout;
	uint32_t			time_start;
	uint32_t			op_time;
}SPI_LCD_DriverStruct_t;

/* flags */

extern	GPIO_TypeDef		*ST7735_cs_port;
extern	uint16_t			ST7735_cs_bit;
extern	GPIO_TypeDef		*ST7735_reset_port;
extern	uint16_t			ST7735_reset_bit;
extern	uint16_t			ST7735_reset_time;
extern	GPIO_TypeDef		*ST7735_dc_port;
extern	uint16_t			ST7735_dc_bit;
extern	SPI_HandleTypeDef 	*ST7735_spi_port;
extern	uint8_t				*ST7735_flags;
extern	uint8_t				*ST7735_dma_timeout;

extern	GPIO_TypeDef		*ILI9341_cs_port;
extern	uint16_t			ILI9341_cs_bit;
extern	GPIO_TypeDef		*ILI9341_reset_port;
extern	uint16_t			ILI9341_reset_bit;
extern	uint16_t			ILI9341_reset_time;
extern	GPIO_TypeDef		*ILI9341_dc_port;
extern	uint16_t			ILI9341_dc_bit;
extern	SPI_HandleTypeDef 	*ILI9341_spi_port;
extern	uint8_t				*ILI9341_flags;
extern	uint8_t				*ILI9341_dma_timeout;

extern	uint16_t	framebuffer_rect[MAX_WIDTH*MAX_HEIGHT];
#include "st7735/st7735.h"
#include "ili9341/ili9341.h"

extern uint32_t	spi_lcd_register(SPI_LCD_DriverStruct_t *spi_lcd_Drv);
extern uint32_t	spi_lcd_on(SPI_LCD_DriverStruct_t *spi_lcd_Drv );
extern uint32_t	spi_lcd_off(SPI_LCD_DriverStruct_t *spi_lcd_Drv );
extern uint32_t	spi_lcd_init(SPI_LCD_DriverStruct_t *spi_lcd_Drv );
extern uint32_t	spi_lcd_clear_screen(SPI_LCD_DriverStruct_t *spi_lcd_Drv );
extern uint32_t	spi_lcd_draw_image(SPI_LCD_DriverStruct_t *spi_lcd_Drv, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* image);



#endif /* DRIVERS_SPI_LCD_LCD_H_ */
