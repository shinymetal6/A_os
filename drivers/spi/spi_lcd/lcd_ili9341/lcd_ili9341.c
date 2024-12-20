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
 *      https://github.com/afiskon/stm32-ili9341/blob/master/Lib/ili9341/ili9341.c#L268
 */


#include "main.h"
#include "../../../../kernel/system_default.h"
#include "../../../../kernel/A.h"
#include "../../../../kernel/A_exported_functions.h"

#ifdef STM32H7xx_HAL_SPI_H
#include "lcd_ili9341.h"

//uint16_t		framebuffer_rect[ILI9341_WIDTH*ILI9341_HEIGHT];
GPIO_TypeDef	*ILI9341_cs_port;
uint16_t		ILI9341_cs_bit;
GPIO_TypeDef	*ILI9341_reset_port;
uint16_t		ILI9341_reset_bit;
uint16_t		ILI9341_reset_time;
GPIO_TypeDef	*ILI9341_dc_port;
uint16_t		ILI9341_dc_bit;
SPI_HandleTypeDef 	*ILI9341_spi_port;
uint8_t			*ILI9341_flags;

static void ILI9341_Select(void)
{
    HAL_GPIO_WritePin(ILI9341_cs_port, ILI9341_cs_bit, GPIO_PIN_RESET);
}

void ILI9341_Unselect(void) {
    HAL_GPIO_WritePin(ILI9341_cs_port, ILI9341_cs_bit, GPIO_PIN_SET);
}

void ILI9341_Reset(void)
{
    HAL_GPIO_WritePin(ILI9341_reset_port, ILI9341_reset_bit, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(ILI9341_reset_port, ILI9341_reset_bit, GPIO_PIN_SET);
}

static void ILI9341_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(ILI9341_dc_port, ILI9341_dc_bit, GPIO_PIN_RESET);
    HAL_SPI_Transmit(ILI9341_spi_port, &cmd, sizeof(cmd), ILI9341_SPI_TIMEOUT);
}

static void ILI9341_WriteData(uint8_t* buff, uint32_t buff_size) {
    HAL_GPIO_WritePin(ILI9341_dc_port, ILI9341_dc_bit, GPIO_PIN_SET);

    // split data in small chunks because HAL can't send more then 64K at once
    while(buff_size > 0)
    {
        uint16_t chunk_size = buff_size > 7679 ? 7680 : buff_size;
        HAL_SPI_Transmit(ILI9341_spi_port, buff, chunk_size, ILI9341_SPI_TIMEOUT);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
}

static void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{

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

void ILI9341_Init() {
    ILI9341_Select();
    ILI9341_Reset();

    // command list is based on https://github.com/martnak/STM32-ILI9341

    // SOFTWARE RESET
    ILI9341_WriteCommand(0x01);
    HAL_Delay(1000);

    // POWER CONTROL A
    ILI9341_WriteCommand(0xCB);
    {
        uint8_t data[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POWER CONTROL B
    ILI9341_WriteCommand(0xCF);
    {
        uint8_t data[] = { 0x00, 0xC1, 0x30 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // DRIVER TIMING CONTROL A
    ILI9341_WriteCommand(0xE8);
    {
        uint8_t data[] = { 0x85, 0x00, 0x78 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // DRIVER TIMING CONTROL B
    ILI9341_WriteCommand(0xEA);
    {
        uint8_t data[] = { 0x00, 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POWER ON SEQUENCE CONTROL
    ILI9341_WriteCommand(0xED);
    {
        uint8_t data[] = { 0x64, 0x03, 0x12, 0x81 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // PUMP RATIO CONTROL
    ILI9341_WriteCommand(0xF7);
    {
        uint8_t data[] = { 0x20 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POWER CONTROL,VRH[5:0]
    ILI9341_WriteCommand(0xC0);
    {
        uint8_t data[] = { 0x23 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POWER CONTROL,SAP[2:0];BT[3:0]
    ILI9341_WriteCommand(0xC1);
    {
        uint8_t data[] = { 0x10 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // VCM CONTROL
    ILI9341_WriteCommand(0xC5);
    {
        uint8_t data[] = { 0x3E, 0x28 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // VCM CONTROL 2
    ILI9341_WriteCommand(0xC7);
    {
        uint8_t data[] = { 0x86 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // MEMORY ACCESS CONTROL
    ILI9341_WriteCommand(0x36);
    {
        uint8_t data[] = { 0x48 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // PIXEL FORMAT
    ILI9341_WriteCommand(0x3A);
    {
        uint8_t data[] = { 0x55 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // FRAME RATIO CONTROL, STANDARD RGB COLOR
    ILI9341_WriteCommand(0xB1);
    {
        uint8_t data[] = { 0x00, 0x18 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // DISPLAY FUNCTION CONTROL
    ILI9341_WriteCommand(0xB6);
    {
        uint8_t data[] = { 0x08, 0x82, 0x27 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // 3GAMMA FUNCTION DISABLE
    ILI9341_WriteCommand(0xF2);
    {
        uint8_t data[] = { 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // GAMMA CURVE SELECTED
    ILI9341_WriteCommand(0x26);
    {
        uint8_t data[] = { 0x01 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // POSITIVE GAMMA CORRECTION
    ILI9341_WriteCommand(0xE0);
    {
        uint8_t data[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                           0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
        ILI9341_WriteData(data, sizeof(data));
    }

    // NEGATIVE GAMMA CORRECTION
    ILI9341_WriteCommand(0xE1);
    {
        uint8_t data[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                           0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
        ILI9341_WriteData(data, sizeof(data));
    }

    // EXIT SLEEP
    ILI9341_WriteCommand(0x11);
    HAL_Delay(120);

    // TURN ON DISPLAY
    ILI9341_WriteCommand(0x29);

    // MADCTL
    ILI9341_WriteCommand(0x36);
    {
        uint8_t data[] = { ILI9341_ROTATION };
        ILI9341_WriteData(data, sizeof(data));
    }

    ILI9341_Unselect();
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT))
        return;

    ILI9341_Select();

    ILI9341_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ILI9341_WriteData(data, sizeof(data));

    ILI9341_Unselect();
}

static void ILI9341_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
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

uint32_t ILI9341_WriteString(uint16_t x, uint16_t y, char* str, FontDef font, uint16_t color, uint16_t bgcolor)
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
    return 0;
}


static void ILI9341_WriteData_DMA(uint8_t* buff, size_t buff_size)
{
    // split data in small chunks because HAL can't send more then 64K at once
    while(buff_size > 0)
    {
        uint16_t chunk_size = buff_size > 32767 ? 32768 : buff_size;
    	*ILI9341_flags &= ~SPI_DMA_DONE;
        HAL_GPIO_WritePin(ILI9341_dc_port, ILI9341_dc_bit, GPIO_PIN_SET);
    	HAL_SPI_Transmit_DMA(ILI9341_spi_port, buff, buff_size);
    	while((*ILI9341_flags & SPI_DMA_DONE) != SPI_DMA_DONE)
    		task_delay(1);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
}
uint32_t te;
uint32_t ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t* data)
{
uint32_t image_size = w*h*2;
    if((x + w - 1) > ILI9341_WIDTH)
    	return 1;
    if((y + h - 1) > ILI9341_HEIGHT)
    	return 1;
    A_get_timelapse_start();
    ILI9341_Select();
    ILI9341_SetAddressWindow(x, y, (x+w), (y+h));


    if ( (*ILI9341_flags & SPI_USES_DMA) == SPI_USES_DMA)
    	ILI9341_WriteData_DMA((uint8_t*)data, image_size);
    else
    	ILI9341_WriteData((uint8_t*)data, image_size);

    ILI9341_Unselect();
    te = A_get_timelapse_end();
    return 0;
}

uint32_t ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
uint32_t	i;
	for(i=0;i<ILI9341_WIDTH*ILI9341_HEIGHT;i++)
		framebuffer_rect[i] = color;
	return ILI9341_DrawImage(x,y,w,h,framebuffer_rect);
}

void ILI9341_FillScreen(uint16_t color)
{
uint32_t	i;
	for(i=0;i<ILI9341_WIDTH*ILI9341_HEIGHT;i++)
		framebuffer_rect[i] = color;
	ILI9341_DrawImage(0,0,ILI9341_WIDTH,ILI9341_HEIGHT,framebuffer_rect);
}

void ILI9341_ClearScreen(void)
{
	ILI9341_FillRectangle(0,0,ILI9341_WIDTH,ILI9341_HEIGHT,ILI9341_BLACK);
}

uint32_t ILI9341_InvertColors(uint8_t invert)
{
    ILI9341_Select();
    ILI9341_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
    ILI9341_Unselect();
	return LCD_OK;
}

#endif // #ifdef STM32H7xx_HAL_SPI_H
