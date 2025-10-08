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
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"

#include "gpio.h"
void set_gpio_mode(GPIO_TypeDef	*gpio_port,uint16_t	gpio_bit,uint8_t mode,uint8_t value)
{
	uint32_t	shft;
	for(shft=0;shft<16;shft++)
		if ( gpio_bit == 1<<shft )
			break;
	shft *= 2;

	switch( mode )
	{
	case	MODE_INPUT 		:
		gpio_port->MODER &= ~(3 << shft);
		gpio_port->MODER |= MODE_INPUT<<shft;
		break;
	case	MODE_AF 	:
		gpio_port->MODER &= ~(3 << shft);
		gpio_port->MODER |= MODE_AF<<shft;
		break;
	case	MODE_OUTPUT	 	:
		gpio_port->MODER &= ~(3 << shft);
		gpio_port->MODER |= MODE_OUTPUT<<shft;
		if (value)
			gpio_port->BSRR = (uint32_t)gpio_bit;
		else
			gpio_port->BRR = (uint32_t)gpio_bit;
		break;
	case	MODE_ANALOG	 	:
		gpio_port->MODER &= ~(3 << shft);
		gpio_port->MODER |= MODE_ANALOG<<shft;
		break;
	default	 	: break;
	}
}
