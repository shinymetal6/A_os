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
 * gpio_irq.h
 *
 *  Created on: Oct 29, 2024
 *      Author: fil
 */
#ifndef DRIVERS_GPIO_GPIO_IRQ_H_
#define DRIVERS_GPIO_GPIO_IRQ_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	GPIO_TypeDef 		*GPIO_Port;
	uint16_t 			GPIO_Pin;
	uint8_t 			process;
	uint32_t 			wakeup_id;
	void 				(*exti_irq_callback)(uint16_t GPIO_Pin);
}OnChip_GPIO_Irq_DriverStruct_t;

/* flags : watchout speed !!*/
#define	GPIO_IS_LOWSPEED	0x00
#define	GPIO_IS_MEDIUMSPEED	0x01
#define	GPIO_IS_FASTSPEED	0x02
#define	GPIO_IS_HIGHSPEED	0x03
#define	GPIO_HAS_PULLUP		0x04
#define	GPIO_HAS_PULLOWN	0x08
#define	GPIO_IS_TP			0x10
#define	GPIO_IS_OC			0x20
#define	GPIO_IS_OUTPUT		0x40
#define	GPIO_IS_ALTERNATE	0x80
#define	GPIO_IS_ANALOG		0xC0
#define	GPIO_IS_INPUT		0x00

#define	GPIO_DRV_MODEMASK		0xc0

#define	GPIO_DRV_SPEEDMASK		0x03
#define	GPIO_DRV_SPEED_LOW		0x00
#define	GPIO_DRV_SPEED_MEDIUM	0x01
#define	GPIO_DRV_SPEED_HIGH		0x10

extern	uint32_t driver_gpio_allocate_driver(GPIO_Irq_DriverStruct_t *new_struct);


#endif /* DRIVERS_GPIO_GPIO_IRQ_H_ */
