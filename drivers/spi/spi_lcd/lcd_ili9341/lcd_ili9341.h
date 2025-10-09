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
 * lcd_ili9341.h
 *
 *  Created on: Feb 16, 2024
 *      Author: fil
 */

#ifndef DRIVERS_LCD_ILI9341_LCD_ILI9341_H_
#define DRIVERS_LCD_ILI9341_LCD_ILI9341_H_

#include "main.h"
#ifdef A_OS_SPI_ENABLED
#ifdef LCD_ENABLED

#include "lcd_ili9341.h"
#include "../fonts.h"

#define	LCD_NOT_OWNED	1
#define	LCD_OK			0

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

#define	ZERO_BRIGHTNESS		0
#define	LOW_BRIGHTNESS		30
#define	HALF_BRIGHTNESS		500
#define	Hi_BRIGHTNESS		750
#define	FULL_BRIGHTNESS		1000
extern	TIM_HandleTypeDef 	htim16;
#define	BACKLIGHT_TIMER		htim16

// default orientation

#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)

// rotate right
/*
#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)
*/

// rotate left
/*
#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240
#define ILI9341_ROTATION (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)
*/

// upside down
/*
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
#define ILI9341_ROTATION (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR)
*/

/****************************/

#define	FBSIZE			32768

// Color definitions
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x001F
#define	ILI9341_RED     0xF800
#define	ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF
#define ILI9341_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))
#define	ILI9341_SPI_TIMEOUT	10

// call before initializing any SPI devices
void ILI9341_Unselect();
extern	void ILI9341_Init(void);
extern	void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
extern	uint32_t ILI9341_WriteString(uint16_t x, uint16_t y, char* str, FontDef font, uint16_t color, uint16_t bgcolor);
extern	uint32_t ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
extern	void ILI9341_FillScreen(uint16_t color);
extern	uint32_t ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* data);
extern	uint32_t ILI9341_InvertColors(uint8_t invert);
extern	void ILI9341_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
extern	void ILI9341_Reset(void);
extern	void ILI9341_ClearScreen(void);

#endif // #ifdef LCD_ENABLED
#endif // #ifdef A_OS_SPI_ENABLED

#endif /* DRIVERS_LCD_ILI9341_LCD_ILI9341_H_ */
