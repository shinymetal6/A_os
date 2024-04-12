/*
 * gpio.c
 *
 *  Created on: Apr 12, 2024
 *      Author: Luigi Fazio
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"

#include "gpio.h"

GPIO_TypeDef* gpio_oneWire_PORT;
uint16_t gpio_oneWire_PIN;
uint8_t gpio_oneWirePin_Idx;

uint8_t Get_Idx(GPIO_TypeDef* DataPort, uint16_t DataPin)
{
	gpio_oneWire_PORT = DataPort;
	gpio_oneWire_PIN = DataPin;
	for(uint8_t i=0; i<16; i++)
	{
		if(DataPin & (1 << i))
		{
			gpio_oneWirePin_Idx = i;
			break;
		}
	}
	return gpio_oneWirePin_Idx;
}

void GPIO_SetMODER(GPIO_TypeDef *gpio,  uint16_t DataPin, GPIO_MODER mode)
{

	uint8_t pin_iDX = Get_Idx(gpio,DataPin);
	uint32_t reg = gpio -> MODER;

    reg &= ~(0b11 << (pin_iDX * 2));
    reg |= (mode & 0b11) << (pin_iDX * 2);

    gpio -> MODER = reg;
}

void GPIO_SetPUPDR(GPIO_TypeDef *gpio,  uint16_t DataPin, GPIO_PUPDR mode)
{
	uint8_t pin_iDX = Get_Idx(gpio,DataPin);
	uint32_t reg = gpio -> PUPDR;

    reg &= ~(0b11 << (pin_iDX * 2));
    reg |= (mode & 0b11) << (pin_iDX * 2);

    gpio -> PUPDR = reg;
}


void GPIO_SetOSPEEDR(GPIO_TypeDef *gpio,  uint16_t DataPin, GPIO_OSPEEDR mode)
{
	uint8_t pin_iDX = Get_Idx(gpio,DataPin);
	uint32_t reg = gpio -> OSPEEDR;

    reg &= ~(0b11 << (pin_iDX * 2));
    reg |= (mode & 0b11) << (pin_iDX * 2);

    gpio -> OSPEEDR = reg;
}
