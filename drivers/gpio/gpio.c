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
 * gpio.c
 *
 *  Created on: Nov 13, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#include "gpio.h"

ITCM_AREA_CODE static uint32_t get_shift_by_pin(uint16_t	gpio_bit)
{
uint32_t	shft;
	for(shft=0;shft<16;shft++)
		if ( gpio_bit == 1<<shft )
			break;
	return shft * 2;
}

ITCM_AREA_CODE void set_gpio_type(GPIO_TypeDef	*gpio_port,uint16_t	gpio_bit,uint16_t	otype,uint16_t	pupd,uint16_t	speed )
{
uint32_t	shft = get_shift_by_pin(gpio_bit);
	// --- Output type (OTYPER) --- Possible types : OUTPUT_OD OUTPUT_PP
	if (otype == OUTPUT_OD)
		gpio_port->OTYPER |= gpio_bit;
	else
		gpio_port->OTYPER &= ~gpio_bit;

	// --- Pull-up/pull-down (PUPDR) --- Possible types : GPIO_NOPULL GPIO_PULLUP GPIO_PULLDOWN
	gpio_port->PUPDR &= ~(0x3U << shft);
	gpio_port->PUPDR |= ((uint32_t)pupd << shft);

	// --- Speed (OSPEEDR) --- Possible types : GPIO_SPEED_FREQ_LOW GPIO_SPEED_FREQ_MEDIUM GPIO_SPEED_FREQ_HIGH GPIO_SPEED_FREQ_VERY_HIGH
	gpio_port->OSPEEDR &= ~(0x3U << shft);
	gpio_port->OSPEEDR |= ((uint32_t)speed << shft);
}

ITCM_AREA_CODE void set_gpio_mode(GPIO_TypeDef	*gpio_port,uint16_t	gpio_bit,uint8_t mode,uint8_t value)
{
uint32_t	shft = get_shift_by_pin(gpio_bit);
	switch( mode )
	{
	case	MODE_INPUT 		:
		gpio_port->MODER &= ~(0x3U << shft);
		gpio_port->MODER |= MODE_INPUT<<shft;
		break;
	case	MODE_AF 	:
		gpio_port->MODER &= ~(0x3U << shft);
		gpio_port->MODER |= MODE_AF<<shft;
		break;
	case	MODE_OUTPUT	 	:
		gpio_port->MODER &= ~(0x3U << shft);
		gpio_port->MODER |= MODE_OUTPUT<<shft;
#ifdef STM32H743xx
		if (value)
			gpio_port->BSRR = gpio_bit;
		else
			gpio_port->BSRR = (uint32_t)gpio_bit << 16U;
#else
		if (value)
			gpio_port->BSRR = (uint32_t)gpio_bit;
		else
			gpio_port->BRR = (uint32_t)gpio_bit;
#endif
		break;
	case	MODE_ANALOG	 	:
		gpio_port->MODER &= ~(0x3U << shft);
		gpio_port->MODER |= MODE_ANALOG<<shft;
		break;
	default	 	: break;
	}
}
