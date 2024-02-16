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
 * fonts_ili9341.h
 *
 *  Created on: Feb 16, 2024
 *      Author: fil
 */

#ifndef DRIVERS_LCD_ILI9341_FONTS_ILI9341_H_
#define DRIVERS_LCD_ILI9341_FONTS_ILI9341_H_

#include <stdint.h>

typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} ili9341_FontDef;


extern ili9341_FontDef ili9341_Font_7x10;
extern ili9341_FontDef ili9341_Font_11x18;
extern ili9341_FontDef ili9341_Font_16x26;


#endif /* DRIVERS_LCD_ILI9341_FONTS_ILI9341_H_ */
