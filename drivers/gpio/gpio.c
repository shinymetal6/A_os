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
void set_gpio_mode(GPIO_TypeDef	*one_wire_port,uint16_t	one_wire_bit,uint8_t mode,uint8_t value)
{
uint32_t	one_wire_32 = (uint32_t )one_wire_bit;
	switch( mode )
	{
	case	GPIO_IS_INPUT 		:
		one_wire_port->MODER &= ~(0x03 << (one_wire_32 << 1));
		one_wire_port->PUPDR &= ~(0x03 << (one_wire_32 << 1));
		break;
	case	GPIO_IS_ALTERNATE 	:
		one_wire_port->MODER &= ~(0x03 << (one_wire_32 << 1));
		one_wire_port->MODER |= 1 << (((one_wire_32+1) * 2)-1);
		break;
	case	GPIO_IS_OUTPUT	 	:
		one_wire_port->MODER &= ~(0x03 << (one_wire_32 << 1));
		one_wire_port->MODER |= 1 << (((one_wire_32+1) * 2)-2);
		one_wire_port->BSRR = (value != GPIO_PIN_RESET) ? one_wire_bit : (uint32_t)one_wire_bit << (16U);
		break;
	case	GPIO_IS_ANALOG	 	:
		one_wire_port->MODER |= 0x03 << (one_wire_32 << 1);
		break;
	default	 	: break;
	}
}
