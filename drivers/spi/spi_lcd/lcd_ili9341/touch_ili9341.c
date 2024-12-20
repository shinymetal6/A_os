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
 * touch_ili9341.c
 *
 *  Created on: Feb 16, 2024
 *      Author: fil
 */


/* vim: set ai et ts=4 sw=4: */

#include "main.h"
#include "../../kernel/system_default.h"
#ifdef	LCD_2I8_ENABLED

#define READ_X 0xD0
#define READ_Y 0x90


void ILI9341_TouchCallback(void)
{

}

static void ILI9341_TouchSelect(void)
{
    HAL_GPIO_WritePin(ILI9341_TOUCH_CS_GPIO_Port, ILI9341_TOUCH_CS_Pin, GPIO_PIN_RESET);
}

void ILI9341_TouchUnselect(void)
{
    HAL_GPIO_WritePin(ILI9341_TOUCH_CS_GPIO_Port, ILI9341_TOUCH_CS_Pin, GPIO_PIN_SET);
}

uint32_t ILI9341_TouchPressed(void)
{
    return HAL_GPIO_ReadPin(ILI9341_TOUCH_IRQ_GPIO_Port, ILI9341_TOUCH_IRQ_Pin) == GPIO_PIN_RESET;
}

uint32_t ILI9341_TouchGetCoordinates(uint32_t* raw_x, uint32_t* raw_y)
{
    static const uint8_t cmd_read_x[] = { READ_X };
    static const uint8_t cmd_read_y[] = { READ_Y };
    static const uint8_t zeroes_tx[] = { 0x00, 0x00 };
    uint32_t	mbr , touch_mbr;

    /* set prescaler to 128 */
    mbr = hspi1.Instance->CFG1;
    touch_mbr = mbr & 0x8fffffff;
    touch_mbr |= 0xe0000000;
    hspi1.Instance->CFG1 = touch_mbr;

    ILI9341_TouchSelect();

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;
    uint8_t nsamples = 0;
    for(uint8_t i = 0; i < 16; i++) {
        if(!ILI9341_TouchPressed())
            break;

        nsamples++;

        HAL_SPI_Transmit(&ILI9341_TOUCH_SPI_PORT, (uint8_t*)cmd_read_y, sizeof(cmd_read_y), HAL_MAX_DELAY);
        uint8_t y_raw[2];
        HAL_SPI_TransmitReceive(&ILI9341_TOUCH_SPI_PORT, (uint8_t*)zeroes_tx, y_raw, sizeof(y_raw), HAL_MAX_DELAY);

        HAL_SPI_Transmit(&ILI9341_TOUCH_SPI_PORT, (uint8_t*)cmd_read_x, sizeof(cmd_read_x), HAL_MAX_DELAY);
        uint8_t x_raw[2];
        HAL_SPI_TransmitReceive(&ILI9341_TOUCH_SPI_PORT, (uint8_t*)zeroes_tx, x_raw, sizeof(x_raw), HAL_MAX_DELAY);

        avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    ILI9341_TouchUnselect();
    hspi1.Instance->CFG1 = mbr;

    if(nsamples < 16)
        return 0;

    *raw_x = (avg_x / 16);
    *raw_y = (avg_y / 16);

    return 1;
}

uint32_t ILI9341_GetTouch(uint32_t* raw_x, uint32_t* raw_y)
{
uint32_t ret_val = 0;
/*
	ret_val = ILI9341_TouchGetCoordinates(raw_x, raw_y);
	if ( *raw_x < SystemParameters.touch_x_minimum )
		*raw_x = 0;
	else
		*raw_x -= SystemParameters.touch_x_minimum;
	*raw_x /= SystemParameters.touch_x_correction;
	if ( *raw_y < SystemParameters.touch_y_minimum )
		*raw_y = 0;
	else
		*raw_y -= SystemParameters.touch_y_minimum;
	*raw_y /= SystemParameters.touch_y_correction;
	*/
	return ret_val;
}

#endif // #ifdef	LCD_2I8_ENABLED

