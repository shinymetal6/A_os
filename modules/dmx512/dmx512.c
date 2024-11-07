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
 * dmx512.c
 *
 *  Created on: Mar 23, 2024
 *      Author: fil
 */


#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"

#include "dmx512.h"
extern	void DWT_Delay_us(uint32_t au32_microseconds);
extern		UARTS_DriverStruct_t	*UARTS_DriverStruct[MAX_UARTS_DRIVERS];

uint32_t dmx512_tx(uint8_t handle)
{
Dmx_control_TypeDef	*Dmx_control = (Dmx_control_TypeDef	*)UARTS_DriverStruct[handle]->uart_driver_private_data;
	gpio_driver_gpio_configure(Dmx_control->gpio_uart_bit_driver_handle,GPIO_IS_OUTPUT);
	DWT_Delay_us(Dmx_control->break_len);
	gpio_driver_gpio_configure(Dmx_control->gpio_uart_bit_driver_handle,GPIO_IS_ALTERNATE);
	return uart_driver_send_buffer(Dmx_control->dmx_uart_handle,Dmx_control->DMXbuf,DMX_LEN);
}

uint32_t dmx512_init(Dmx_control_TypeDef *Dmx_control)
{

OnChip_GPIO_Irq_DriverStruct_t	GPIO_DmxUart =
{
		.GPIO_Port = Dmx_control->uart_port,
		.GPIO_Pin = Dmx_control->uart_bit,
};

	driver_gpio_allocate_driver(Dmx_control->GpIoO1);
	Dmx_control->gpio_uart_bit_driver_handle = gpio_driver_register(Dmx_control->GpIoO1,(uint32_t *)&GPIO_DmxUart,0);
	gpio_driver_gpio_set(Dmx_control->gpio_uart_bit_driver_handle,0);
	onchip_uart_allocate_driver(Dmx_control->dmx_uart);
	Dmx_control->dmx_uart->uart_driver_private_data = (uint32_t *)Dmx_control;
	Dmx_control->dmx_uart_handle = uart_driver_register(Dmx_control->dmx_uart,(uint32_t *)Dmx_control,DRIVER_FLAGS_AUTOSTART,Dmx_control->flags);;
	return Dmx_control->dmx_uart_handle;
}


