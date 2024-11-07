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
 * lcd_7735.h
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#ifndef DRIVERS_LCD_ST7735_LCD_7735_H_
#define DRIVERS_LCD_ST7735_LCD_7735_H_

#include "main.h"

#include <stdio.h>
#include <string.h>

#define	LCD_NOT_OWNED	1
#define	LCD_OK			0

typedef struct
{
	uint8_t xpos;
	uint8_t	ypos;
    char line[32];
    uint16_t fore_color;
	uint16_t bkg_color;
	uint16_t current_brightness;
} Video_t;

#define	ZERO_BRIGHTNESS	0
#define	LOW_BRIGHTNESS	30
#define	HALF_BRIGHTNESS	500
#define	FULL_BRIGHTNESS	1000

#define CHAR_W 		11
#define CHAR_H 		18
#define MIN_TIME	10
#define MAX_TIME	180
#define TIME_STEP	10
#define START_TIME	30
#define STOPPED		0
#define RUNNING		1
#define DEBOUNCE_TIME 10
#define TIM14_COUNTS 10
#define TIM17_COUNTS 1000
#define	LIMIT		500
#define	STEP		10
#define	RAMP_STEP	20
#define	FINISHED_TIME_BANNER	200

extern	TIM_HandleTypeDef 	htim16;
#define	BACKLIGHT_TIMER		htim16

extern	uint32_t lcd_control_096_allocate_driver(DriverStruct_t *new_struct);

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	TIM_HandleTypeDef 	*backlight_timer;
	uint16_t 			backlight_timer_channel;
	uint16_t 			brightness;
	SPI_HandleTypeDef 	*spi;
	GPIO_TypeDef	 	*ss_port;
	uint16_t			ss_bit;
	GPIO_TypeDef	 	*dc_port;
	uint16_t			dc_bit;
	GPIO_TypeDef	 	*reset_port;
	uint16_t			reset_bit;
}Lcd_Control_096_Drv_TypeDef;
#define	LCD_096_RUNNING	0x80

typedef struct
{
	uint8_t				action;
	char				*string;
	uint16_t			string_len;
	FontDef 			font;
	uint16_t			pos_x;
	uint16_t			pos_y;
	uint16_t 			color;
	uint16_t 			bgcolor;
}Lcd_Control_096_Actions_TypeDef;

/* extended commands */
#define	LCD_096_SET_BRIGHTNESS		1
#define	LCD_096_START_BACKLIGHT		2
#define	LCD_096_STOP_BACKLIGHT		3
#define	LCD_096_CLEAR_SCREEN		4
#define	LCD_096_DRAW_LOGO			5
#define	LCD_096_WRITE_STRING		6

#endif /* DRIVERS_LCD_ST7735_LCD_7735_H_ */
