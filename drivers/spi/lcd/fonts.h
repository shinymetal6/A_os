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
 * fonts.h
 *
 *  Created on: Nov 7, 2025
 *      Author: fil
 */

#ifndef DRIVERS_SPI_LCD_FONTS_H_
#define DRIVERS_SPI_LCD_FONTS_H_

typedef struct {
    const uint8_t width;
    const uint8_t height;
    const uint16_t *data;
} FontDef;


extern  FontDef Font_7x10;
extern 	FontDef Font_7x11;
extern  FontDef Font_11x18;
extern 	FontDef Font_16x26;

#endif /* DRIVERS_SPI_LCD_FONTS_H_ */
