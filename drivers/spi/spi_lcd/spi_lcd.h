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
 * spi_lcd.h
 *
 *  Created on: Nov 23, 2024
 *      Author: fil
 */

#ifndef DRIVERS_SPI_SPI_LCD_SPI_LCD_H_
#define DRIVERS_SPI_SPI_LCD_SPI_LCD_H_

#define	LCD_IS_7735		0x7735
#define	LCD_IS_9341		0x9341

#define	DEFAULT_RESET_TIME	100
typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	TIM_HandleTypeDef	*backlight_timer;
	uint32_t			backlight_timer_channel;
	SPI_HandleTypeDef 	*bus;
	GPIO_TypeDef	 	*cs_port;
	uint16_t			cs_bit;
	GPIO_TypeDef	 	*reset_port;
	uint16_t			reset_bit;
	uint16_t			reset_time;
	uint8_t				active_level_reset_bit;
	GPIO_TypeDef	 	*dc_port;
	uint16_t			dc_bit;
	uint16_t			lcd_model;
	uint16_t			current_brightness;
	void				(*lcd_init)(void);
	void				(*lcd_reset)(void);
	void				(*lcd_clear_screen)  (void);
	uint32_t			(*lcd_invert_colors)  (uint8_t handle,uint8_t invert);
	uint32_t			(*lcd_write_char)    (uint16_t x, uint16_t y, char ch,    uint8_t font_index, uint16_t color, uint16_t bgcolor);
	uint32_t			(*lcd_write_string)  (uint16_t x, uint16_t y, char *line, uint8_t font_index, uint16_t color, uint16_t bgcolor);
	uint32_t			(*lcd_fill_rect)  (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	uint32_t			(*lcd_draw_image)  (uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
}SPI_LCD_DriverStruct_t;

extern	GPIO_TypeDef		*ST7735_cs_port;
extern	uint16_t			ST7735_cs_bit;
extern	GPIO_TypeDef		*ST7735_reset_port;
extern	uint16_t			ST7735_reset_bit;
extern	uint16_t			ST7735_reset_time;
extern	GPIO_TypeDef		*ST7735_dc_port;
extern	uint16_t			ST7735_dc_bit;
extern	SPI_HandleTypeDef 	*ST7735_spi_port;

extern uint32_t	spi_lcd_init(uint8_t handle);
extern uint32_t	spi_lcd_register(SPI_LCD_DriverStruct_t *driver_private_data);
extern uint32_t	spi_lcd_clear_screen(uint8_t handle);
extern uint32_t	spi_lcd_fill_rect(uint8_t handle,uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern uint32_t	spi_lcd_reset(uint8_t handle);
extern uint32_t	spi_lcd_set_brightness(uint8_t handle,uint16_t brightness);



#include	"st7735_lcd/lcd_7735.h"

#endif /* DRIVERS_SPI_SPI_LCD_SPI_LCD_H_ */
