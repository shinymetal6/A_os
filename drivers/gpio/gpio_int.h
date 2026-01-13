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
 * gpio_int.h
 *
 *  Created on: Jan 13, 2026
 *      Author: fil
 */

#ifndef DRIVERS_GPIO_GPIO_INT_H_
#define DRIVERS_GPIO_GPIO_INT_H_

#ifdef A_OS_GPIO_ENABLED

#define GPIO_INT_WAKEUP_ON_EVENT	0x01

typedef struct
{
	/* driver header */
	uint8_t				status;
	uint8_t				flags;
	uint8_t 			process;
	uint32_t 			wakeup_id;
	uint32_t			*next_drv;
	uint16_t			IRQ_bit;
	GPIO_TypeDef	 	*IRQ_port;
	uint16_t			IRQ_type;
	uint16_t			sampled_bit;
	void				(*irq_exti_callback)  (uint16_t GPIO_Pin,uint32_t *irq_origin_struct_ptr);
	uint32_t			*irq_origin_struct_ptr;
}GPIO_Interrupt_DriverStruct_t;
/* status */
#define GPIO_INT_EVENT				0x01
/* irq_type */
#define	GPIO_INT_TYPE_RISING		0x80
#define	GPIO_INT_TYPE_FALLING		0x40

extern uint32_t	gpio_int_register(GPIO_Interrupt_DriverStruct_t *gpio_irq_Drv);

#endif /* DRIVERS_GPIO_INT_GPIO_INT_H_ */

#endif /* DRIVERS_GPIO_GPIO_INT_H_ */
