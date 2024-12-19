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
 *  Created on: Dec 7, 2024
 *      Author: fil
 */
#ifndef DRIVERS_GPIO_INT_GPIO_INT_H_
#define DRIVERS_GPIO_INT_GPIO_INT_H_

typedef struct
{
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint16_t			irq_bit;
	GPIO_TypeDef	 	*irq_port;
	uint16_t			data_bit;
	GPIO_TypeDef	 	*data_port;
	uint16_t			sampled_bit;
	uint16_t			irq_type;
	uint32_t			wakeup_id;
	void				(*irq_exti_callback)  (uint16_t GPIO_Pin);
	uint16_t			hide_time;
	uint16_t			hide_time_counter;
	uint16_t			irq_index;
	uint32_t			*driver_private_data;
}GPIO_Int_DriverStruct_t;
/* flags */
#define	GPIO_INT_HIDE_ENABLED	0x80
/* irq_type */
#define	GPIO_INT_TYPE_RISING	0x80
#define	GPIO_INT_TYPE_FALLING	0x40

extern uint32_t	gpio_int_register(GPIO_Int_DriverStruct_t *driver_private_data);

#endif /* DRIVERS_GPIO_INT_GPIO_INT_H_ */
