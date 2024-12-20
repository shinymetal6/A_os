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
 * touch_ili9341.h
 *
 *  Created on: Feb 16, 2024
 *      Author: fil
 */

#ifndef DRIVERS_LCD_ILI9341_TOUCH_ILI9341_H_
#define DRIVERS_LCD_ILI9341_TOUCH_ILI9341_H_

/* vim: set ai et ts=4 sw=4: */
#ifndef __ILI9341_TOUCH_H__
#define __ILI9341_TOUCH_H__

#include <stdbool.h>
#include "main.h"

/*** Redefine if necessary ***/

// Warning! Use SPI bus with < 1.3 Mbit speed, better ~650 Kbit to be save.
#define ILI9341_TOUCH_SPI_PORT hspi1
extern SPI_HandleTypeDef ILI9341_TOUCH_SPI_PORT;

#define ILI9341_TOUCH_IRQ_Pin       TOUCH_IRQ_Pin // Arduino D5
#define ILI9341_TOUCH_IRQ_GPIO_Port TOUCH_IRQ_GPIO_Port
#define ILI9341_TOUCH_CS_Pin        TOUCH_CS_Pin // Arduino D2
#define ILI9341_TOUCH_CS_GPIO_Port  TOUCH_CS_GPIO_Port

// change depending on screen orientation
#define ILI9341_TOUCH_SCALE_X 240
#define ILI9341_TOUCH_SCALE_Y 320

// to calibrate uncomment UART_Printf line in ili9341_touch.c
#define ILI9341_TOUCH_MIN_RAW_X 1500
#define ILI9341_TOUCH_MAX_RAW_X 31000
#define ILI9341_TOUCH_MIN_RAW_Y 3276
#define ILI9341_TOUCH_MAX_RAW_Y 30110

// call before initializing any SPI devices
void ILI9341_TouchUnselect();

uint32_t ILI9341_TouchPressed();
uint32_t ILI9341_TouchGetCoordinates(uint32_t* raw_x, uint32_t* raw_y);
void ILI9341_TouchCallback(void);
uint32_t ILI9341_GetTouch(uint32_t* raw_x, uint32_t* raw_y);

extern	uint32_t	touch_flag;



#endif // __ILI9341_TOUCH_H__


#endif /* DRIVERS_LCD_ILI9341_TOUCH_ILI9341_H_ */
