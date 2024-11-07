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
 * gpio_irq_drivers_manager.h
 *
 *  Created on: Oct 29, 2024
 *      Author: fil
 */
#ifndef DRIVERS_GPIO_IRQ_DRIVERS_MANAGER_H_
#define DRIVERS_GPIO_IRQ_DRIVERS_MANAGER_H_

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	(*gpio_set)(uint8_t handle, uint8_t level);
	uint32_t	(*gpio_toggle)(uint8_t handle);
	uint32_t	(*gpio_get)(uint8_t handle);
	uint32_t	(*gpio_configure)(uint8_t handle, uint8_t configuration);
	uint32_t	*gpio_driver_private_data;
	char		gpio_driver_name[32];
}GPIO_Irq_DriverStruct_t;

typedef struct
{
	uint8_t 	in_use;
	uint16_t	gpiobit;
}GPIO_Irq_DriverPortAllocationStruct_t;

#define	PIN_ALREADY_ALLOCATED	255

#include "gpio_irq/gpio_irq.h"

extern	uint32_t gpio_driver_register(GPIO_Irq_DriverStruct_t *driver,uint32_t *private_drv_struct,uint32_t flags);
extern	uint32_t gpio_driver_unregister(GPIO_Irq_DriverStruct_t *driver);
extern	uint32_t gpio_driver_scan(void);
extern	uint32_t gpio_driver_gpio_set(uint32_t handle,uint8_t level);
extern	uint32_t gpio_driver_gpio_toggle(uint32_t handle);
extern	uint32_t gpio_driver_gpio_get(uint32_t handle);
extern	uint32_t gpio_driver_gpio_configure(uint32_t handle, uint8_t configuration);
extern	uint32_t gpio_driver_init(void);
extern	uint8_t  gpio_driver_allocate_gpio(GPIO_TypeDef *GPIO_Port,uint16_t GPIO_Pin);
extern	uint8_t	 gpio_driver_allocate_multiple_gpio(OnChip_GPIO_Irq_DriverStruct_t *OnChip_GPIO_Irq_DriverStruct,uint16_t num_pin);
extern  uint32_t	gpio_driver_multigpio_register(uint32_t *handle,GPIO_Irq_DriverStruct_t *driver,OnChip_GPIO_Irq_DriverStruct_t *private,uint16_t num_gpio,uint32_t flags);

#endif /* DRIVERS_GPIO_IRQ_DRIVERS_MANAGER_H_ */
