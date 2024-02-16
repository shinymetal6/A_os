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
 * lcd_ili9341.c
 *
 *  Created on: Feb 16, 2024
 *      Author: fil
 */


#include "main.h"
#include "../../kernel/system_default.h"
#ifdef	LCD_2I8_ENABLED
#include "lcd_ili9341.h"
#include "fonts_ili9341.h"

#include <stdlib.h>

uint8_t	fillbuff[FILLSIZE];

static void ILI9341_Select(void)
{
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET);
}

void ILI9341_Unselect(void)
{
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET);
}

static void ILI9341_Reset(void)
{
    HAL_GPIO_WritePin(ILI9341_RES_GPIO_Port, ILI9341_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(ILI9341_RES_GPIO_Port, ILI9341_RES_Pin, GPIO_PIN_SET);
}

static void ILI9341_WriteCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ILI9341_SPI_PORT, &cmd, sizeof(cmd), ILI9341_SPI_TIMEOUT);
}

static void ILI9341_WriteData(uint8_t* buff, size_t buff_size)
{
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);

    // split data in small chunks because HAL can't send more then 64K at once
    while(buff_size > 0) {
        uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
        HAL_SPI_Transmit(&ILI9341_SPI_PORT, buff, chunk_size, ILI9341_SPI_TIMEOUT);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
}

static void ILI9341_WriteDmaData(uint8_t* buff, size_t buff_size)
{
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit_DMA(&ILI9341_SPI_PORT, buff, buff_size);
}

void SPI_TxEnd_Callback(void)
{
	ILI9341_Unselect();
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

static void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // column address set
    ILI9341_WriteCommand(0x2A); // CASET
    {
        uint8_t data[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
        ILI9341_WriteData(data, sizeof(data));
    }

    // row address set
    ILI9341_WriteCommand(0x2B); // RASET
    {
        uint8_t data[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
        ILI9341_WriteData(data, sizeof(data));
    }

    // write to RAM
    ILI9341_WriteCommand(0x2C); // RAMWR
}

void ILI9341_Init(void)
{
    ILI9341_Reset();

    // command list is based on https://github.com/martnak/STM32-ILI9341

    // SOFTWARE RESET
    ILI9341_Select();
    ILI9341_WriteCommand(0x01);
    ILI9341_Unselect();
    HAL_Delay(1000);

    // POWER CONTROL A
    ILI9341_Select();
    ILI9341_WriteCommand(0xCB);
    {
        uint8_t data[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // POWER CONTROL B
    ILI9341_Select();
    ILI9341_WriteCommand(0xCF);
    {
        uint8_t data[] = { 0x00, 0xC1, 0x30 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // DRIVER TIMING CONTROL A
    ILI9341_Select();
    ILI9341_WriteCommand(0xE8);
    {
        uint8_t data[] = { 0x85, 0x00, 0x78 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // DRIVER TIMING CONTROL B
    ILI9341_Select();
    ILI9341_WriteCommand(0xEA);
    {
        uint8_t data[] = { 0x00, 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // POWER ON SEQUENCE CONTROL
    ILI9341_Select();
    ILI9341_WriteCommand(0xED);
    {
        uint8_t data[] = { 0x64, 0x03, 0x12, 0x81 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // PUMP RATIO CONTROL
    ILI9341_Select();
    ILI9341_WriteCommand(0xF7);
    {
        uint8_t data[] = { 0x20 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // POWER CONTROL,VRH[5:0]
    ILI9341_Select();
    ILI9341_WriteCommand(0xC0);
    {
        uint8_t data[] = { 0x23 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // POWER CONTROL,SAP[2:0];BT[3:0]
    ILI9341_Select();
    ILI9341_WriteCommand(0xC1);
    {
        uint8_t data[] = { 0x10 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // VCM CONTROL
    ILI9341_Select();
    ILI9341_WriteCommand(0xC5);
    {
        uint8_t data[] = { 0x3E, 0x28 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // VCM CONTROL 2
    ILI9341_Select();
    ILI9341_WriteCommand(0xC7);
    {
        uint8_t data[] = { 0x86 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // MEMORY ACCESS CONTROL
    ILI9341_Select();
    ILI9341_WriteCommand(0x36);
    {
        uint8_t data[] = { 0x48 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // PIXEL FORMAT
    ILI9341_Select();
    ILI9341_WriteCommand(0x3A);
    {
        uint8_t data[] = { 0x55 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    ILI9341_Select();
    ILI9341_WriteCommand(0xB1);
    {
        uint8_t data[] = { 0x00, 0x18 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // DISPLAY FUNCTION CONTROL
    ILI9341_Select();
    ILI9341_WriteCommand(0xB6);
    {
        uint8_t data[] = { 0x08, 0x82, 0x27 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // 3GAMMA FUNCTION DISABLE
    ILI9341_Select();
    ILI9341_WriteCommand(0xF2);
    {
        uint8_t data[] = { 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // GAMMA CURVE SELECTED
    ILI9341_Select();
    ILI9341_WriteCommand(0x26);
    {
        uint8_t data[] = { 0x01 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // POSITIVE GAMMA CORRECTION
    ILI9341_Select();
    ILI9341_WriteCommand(0xE0);
    {
        uint8_t data[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                           0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // NEGATIVE GAMMA CORRECTION
    ILI9341_Select();
    ILI9341_WriteCommand(0xE1);
    {
        uint8_t data[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                           0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

    // EXIT SLEEP
    ILI9341_Select();
    ILI9341_WriteCommand(0x11);
    ILI9341_Unselect();
    HAL_Delay(120);

    // TURN ON DISPLAY
    ILI9341_Select();
    ILI9341_WriteCommand(0x29);
    ILI9341_Unselect();
    HAL_Delay(1);

    // MADCTL
    ILI9341_Select();
    ILI9341_WriteCommand(0x36);
    {
        uint8_t data[] = { ILI9341_ROTATION };
        ILI9341_WriteData(data, sizeof(data));
    }
    ILI9341_Unselect();
    HAL_Delay(1);

}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT))
        return;

    ILI9341_Select();

    ILI9341_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ILI9341_WriteData(data, sizeof(data));

    ILI9341_Unselect();
}

void ILI9341_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
int32_t dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
int32_t dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
int32_t err = (dx>dy ? dx : -dy)/2, e2;

	for(;;)
	{
		ILI9341_DrawPixel(x0,y0,color);
		if (x0==x1 && y0==y1)
			break;
		e2 = err;
		if (e2 >-dx)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy)
		{
			err += dx;
			y0 += sy;
		}
	}
}
void ILI9341_DrawCircle(uint16_t x0,uint16_t y0,uint16_t radius,uint16_t color)
{
int32_t f = 1 - radius;
int32_t ddF_x = 0;
int32_t ddF_y = -2 * radius;
int32_t x = 0;
int32_t y = radius;

	ILI9341_DrawPixel(x0,y0 + radius,color);
	ILI9341_DrawPixel(x0,y0 - radius,color);
	ILI9341_DrawPixel(x0 + radius,y0,color);
	ILI9341_DrawPixel(x0 - radius,y0,color);

    while(x < y)
    {
        if(f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;

        ILI9341_DrawPixel(x0 + x, y0 + y,color);
        ILI9341_DrawPixel(x0 - x, y0 + y,color);
        ILI9341_DrawPixel(x0 + x, y0 - y,color);
        ILI9341_DrawPixel(x0 - x, y0 - y,color);
        ILI9341_DrawPixel(x0 + y, y0 + x,color);
        ILI9341_DrawPixel(x0 - y, y0 + x,color);
        ILI9341_DrawPixel(x0 + y, y0 - x,color);
        ILI9341_DrawPixel(x0 - y, y0 - x,color);
    }
}

static void ILI9341_WriteChar(uint16_t x, uint16_t y, char ch, ili9341_FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    ILI9341_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ILI9341_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ILI9341_WriteData(data, sizeof(data));
            }
        }
    }
}

void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, ili9341_FontDef font, uint16_t color, uint16_t bgcolor)
{
    ILI9341_Select();

    while(*str) {
        if(x + font.width >= ILI9341_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ILI9341_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ILI9341_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ILI9341_Unselect();
}

void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	uint32_t	i,fillsize,fillbufindex;
    // clipping
    if((x > ILI9341_WIDTH) || (y > ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) w = ILI9341_WIDTH - x;
    if((y + h - 1) >= ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;

    ILI9341_Select();
    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);

    for(i=0;i<h*w*2;i+=2 )
    {
    	fillbuff[i] = color >> 8;
    	fillbuff[i+1] = color & 0xFF;
    }
    fillsize = h*w*2;
    fillbufindex = 0;
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);
    while(fillsize > TXSIZE)
    {
        HAL_SPI_Transmit(&ILI9341_SPI_PORT, &fillbuff[fillbufindex], TXSIZE, ILI9341_SPI_TIMEOUT);
        fillbufindex += TXSIZE;
        fillsize -= TXSIZE;
    }
    if ( fillsize != 0 )
        HAL_SPI_Transmit(&ILI9341_SPI_PORT, &fillbuff[fillsize], fillsize, ILI9341_SPI_TIMEOUT);

    ILI9341_Unselect();
}

void ILI9341_FillScreen(uint16_t color)
{
	ILI9341_FillRectangle(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);
}

void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data)
{
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) return;
    if((y + h - 1) >= ILI9341_HEIGHT) return;

	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
    ILI9341_Select();
    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);
    ILI9341_WriteDmaData((uint8_t*)data, sizeof(uint16_t)*w*h);
}

void ILI9341_InvertColors(bool invert)
{
    ILI9341_Select();
    ILI9341_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
    ILI9341_Unselect();
}

void test_ili(void)
{
	ILI9341_WriteString(0, 0, "CIAO", ili9341_Font_7x10, 0, 0);
}


#endif // #ifdef	LCD_2I8_ENABLED
