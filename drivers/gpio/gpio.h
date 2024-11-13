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
 * gpio.h
 *
 *  Created on: Nov 13, 2024
 *      Author: fil
 */

#ifndef DRIVERS_GPIO_GPIO_H_
#define DRIVERS_GPIO_GPIO_H_

#define	PULL_DOWN		0
#define	PULL_UP			1
#define	PULL_NONE		2

#define	SPEED_LOW		0
#define	SPEED_MEDIUM	1
#define	SPEED_HIGH		2

#define	GPIO_IS_INPUT		0
#define	GPIO_IS_ALTERNATE	1
#define	GPIO_IS_OUTPUT		2
#define	GPIO_IS_ANALOG		3

extern void set_gpio_mode(GPIO_TypeDef	*one_wire_port,uint16_t	one_wire_bit,uint8_t mode,uint8_t value);

#endif /* DRIVERS_GPIO_GPIO_H_ */
