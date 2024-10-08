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
 * Project : bb1xx_743_00
*/
/*
 * gpio.c
 *
 *  Created on: Jan 3, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"

#include "gpio.h"

void GPIO_SetGpioOUT(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
	GPIOx->MODER &= ~(0x03 << (GPIO_Pin<< 1));
	GPIOx->MODER |= 1 << (((GPIO_Pin+1) * 2)-2);
	if (PinState != GPIO_PIN_RESET)
		GPIOx->BSRR = GPIO_Pin;
	else
		GPIOx->BSRR = (uint32_t)GPIO_Pin << 16;
}

void GPIO_SetGpioIN(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint16_t pullup)
{
	GPIOx->MODER &= ~(0x03 << (GPIO_Pin<< 1));
	GPIOx->PUPDR &= ~(0x03 << (GPIO_Pin<< 1));
	switch(pullup)
	{
	case PULL_DOWN : GPIOx->PUPDR |= 1 << (((GPIO_Pin+1) * 2)-1); break;
	case PULL_UP   : GPIOx->PUPDR |= 1 << (((GPIO_Pin+1) * 2)-2); break;
	case PULL_NONE : break;
	}
}

void GPIO_SetGpioAlternate(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIOx->MODER &= ~(0x03 << (GPIO_Pin<< 1));
	GPIOx->MODER |= 1 << (((GPIO_Pin+1) * 2)-1);
}

void GPIO_SetPUPDR(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint16_t pullup)
{
	GPIOx->PUPDR &= ~(0x03 << (GPIO_Pin<< 1));
	switch(pullup)
	{
	case PULL_DOWN : GPIOx->PUPDR |= 1 << (((GPIO_Pin+1) * 2)-1); break; // 10
	case PULL_UP   : GPIOx->PUPDR |= 1 << (((GPIO_Pin+1) * 2)-2); break; // 01
	case PULL_NONE : break; // 00
	}
}

void GPIO_SetOSPEEDR(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint16_t speed)
{
	GPIOx->OSPEEDR &= ~(0x03 << (GPIO_Pin<< 1));
	switch(speed)
	{
	case SPEED_LOW		: GPIOx->OSPEEDR |= 1 << (((GPIO_Pin+1) * 2)-1); break; // 10 -> low
	case SPEED_MEDIUM	: GPIOx->OSPEEDR |= 1 << (((GPIO_Pin+1) * 2)-2); break; // 01 -> medium
	case SPEED_HIGH		: GPIOx->OSPEEDR |= (0x03 << (GPIO_Pin<< 1));    break; // 11 -> high
	default				: GPIOx->OSPEEDR |= 1 << (((GPIO_Pin+1) * 2)-1); break; // 10 -> low
	}
}
