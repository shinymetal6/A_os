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
 * uarts_drivers_manager.h
 *
 *  Created on: Oct 28, 2024
 *      Author: fil
 */
#ifndef DRIVERS_UARTS_DRIVERS_MANAGER_H_
#define DRIVERS_UARTS_DRIVERS_MANAGER_H_

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	(*init)(uint8_t handle);
	uint32_t	(*deinit)(uint8_t handle);
	uint32_t	(*start)(uint8_t handle);
	uint32_t	(*stop)(uint8_t handle);
	uint32_t	(*get_status)(uint8_t handle);
	uint32_t	(*get_values)(uint8_t handle, uint8_t *values,uint8_t values_number);
	uint32_t	(*set_status)(uint8_t handle);
	uint32_t	(*set_values)(uint8_t handle, uint8_t *values,uint8_t values_number);
	uint32_t	(*send_buffer)(uint8_t handle, uint8_t *buffer,uint8_t len);
	uint32_t	(*send_buffer_dma)(uint8_t handle, uint8_t *buffer,uint8_t len);
	uint32_t	(*receive_buffer)(uint8_t handle, uint8_t *buffer,uint8_t rx_buf_max_len);
	uint32_t	(*receive_buffer_sentinel)(uint8_t handle, uint8_t *buffer,uint8_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end);
	uint32_t	(*extended_action)(uint32_t handle,uint32_t *action);
	void 		(*periodic_before_check_timers_callback)(void);
	void 		(*periodic_after_check_timers_callback)(void);
	uint32_t	*uart_driver_private_data;
	char		uart_driver_name[32];
}UARTS_DriverStruct_t;
/* status */
#define	UART_DRIVER_STATUS_IN_USE		0x80
#define	UART_DRIVER_STATUS_INITIALIZED	0x40
#define	UART_DRIVER_STATUS_INITPEND		0x20
#define	UART_DRIVER_STATUS_REQUESTED		0x10
#define	UART_DRIVER_STATUS_FAILED		0x01
#define	UART_DRIVER_STATUS_UNUSED		0
/* flags */
#define	UART_DRIVER_FLAGS_AUTOSTART		0x80

#define	UART_DRIVER_REQUEST_FAILED	0xffffffff


extern	uint32_t 	uart_driver_init(void);
extern	uint32_t	uart_driver_register(UARTS_DriverStruct_t *uart_driver,uint32_t *uart_driver_private_data,uint32_t driver_flags,uint32_t uart_flags);
extern	uint32_t	uart_driver_unregister(UARTS_DriverStruct_t *uart_driver);
extern	uint32_t 	uart_driver_start(uint32_t handle);
extern	uint32_t 	uart_driver_extended_action(uint32_t handle,uint32_t *action);
extern	uint32_t 	uart_driver_get_values(uint32_t handle,uint8_t *values,uint8_t values_number);
extern	uint32_t 	uart_driver_set_values(uint32_t handle,uint8_t *values,uint8_t values_number);
extern	uint32_t 	uart_driver_scan(void);
extern	uint32_t 	uart_driver_send_buffer(uint8_t handle, uint8_t *buffer,uint16_t len);
extern	uint32_t 	uart_driver_send_buffer_dma(uint8_t handle, uint8_t *buffer,uint16_t len);
extern	uint32_t 	uart_driver_receive_buffer(uint8_t handle, uint8_t *buffer,uint16_t rx_buf_max_len);
extern	uint32_t 	uart_driver_receive_buffer_sentinel(uint8_t handle, uint8_t *buffer,uint16_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end);

#include "onchip_uart/onchip_uart.h"

#endif /* DRIVERS_UARTS_DRIVERS_MANAGER_H_ */
