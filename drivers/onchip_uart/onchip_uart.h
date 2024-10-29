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
 * onchip_uart.h
 *
 *  Created on: Oct 28, 2024
 *      Author: fil
 */
#ifndef DRIVERS_DIGITAL_ONCHIP_UART_ONCHIP_UART_H_
#define DRIVERS_DIGITAL_ONCHIP_UART_ONCHIP_UART_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				rx_char;
	uint16_t			rx_index;
	uint16_t			rx_max_len;
	uint16_t			rx_num_chars;
	uint8_t				*data;
	uint16_t			timeout;
	uint16_t			timeout_reload_value;
	uint8_t				sentinel_start;
	uint8_t				sentinel_end;
	uint32_t			uart_error;
	UART_HandleTypeDef 	*uart;
	uint32_t 			wakeup_id;
}OnChip_UART_Drv_TypeDef;
/* flags */
#define	UART_WAKEUP_ON_TX			0x80
#define	UART_WAKEUP_ON_RXCHAR		0x40
#define	UART_WAKEUP_ON_RXFULL		0x20
#define	UART_WAKEUP_ON_TIMEOUT		0x10
#define	UART_WAKEUP_ON_ERRORS		0x08
#define	UART_SENTINEL_END_FOUND		0x02
#define	UART_SENTINEL_START_FOUND	0x01

extern	uint32_t onchip_uart_allocate_driver(UARTS_DriverStruct_t *new_struct);

#endif /* DRIVERS_DIGITAL_ONCHIP_UART_ONCHIP_UART_H_ */
