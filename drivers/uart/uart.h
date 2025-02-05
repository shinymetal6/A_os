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
 * uart.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_UART_UART_H_
#define DRIVERS_UART_UART_H_
#ifdef A_OS_UART_ENABLED

/* Private area defs */
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
	uint8_t				sentinel_flags;
	uint32_t			uart_error;
	UART_HandleTypeDef 	*uart;
	GPIO_TypeDef	 	*tx_port;
	uint16_t			tx_bit;
	GPIO_TypeDef	 	*rx_port;
	uint16_t			rx_bit;
	uint32_t			param0;
	uint32_t 			wakeup_id;
}UART_Drv_TypeDef;
/* status */
#define	UART_DRIVER_STATUS_IN_USE		0x80
#define	UART_DRIVER_STATUS_INITIALIZED	0x40
#define	UART_DRIVER_STATUS_INITPEND		0x20
#define	UART_DRIVER_STATUS_REQUESTED	0x10
#define	UART_DRIVER_STATUS_FAILED		0x01
#define	UART_DRIVER_STATUS_UNUSED		0

/* flags */
#define	UART_USES_DMA_TX			0x80
#define	UART_USES_DMA_RX			0x40
#define	UART_WAKEUP_ON_TX			0x20
#define	UART_WAKEUP_ON_RXCHAR		0x10
#define	UART_WAKEUP_ON_RXFULL		0x08
#define	UART_WAKEUP_ON_TIMEOUT		0x04
#define	UART_WAKEUP_ON_ERRORS		0x02
// #define	unused					0x01

/* sentinel_flags */
#define	UART_SENTINEL_END_FOUND		0x02
#define	UART_SENTINEL_START_FOUND	0x01

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	UART_Drv_TypeDef	*driver_private_data;
}UARTS_DriverStruct_t;

extern  uint32_t 	uart_init(uint8_t handle);
extern  uint32_t 	uart_get_status(uint8_t handle);
extern  uint32_t	uart_send(uint8_t handle, uint8_t *buffer,uint16_t len);
extern  uint32_t	uart_start_receive(uint8_t handle);
extern  uint32_t	uart_get_rxlen(uint8_t handle);
extern  uint32_t	uart_set_rxlen(uint8_t handle,uint16_t rx_max_len);
extern  uint32_t	uart_set_sentinel(uint8_t handle, uint8_t sentinel_start, uint8_t sentinel_end);
extern 	uint32_t	uart_register(UART_Drv_TypeDef *uart_driver_private_data);
extern 	void 		UART_Driver_RxTimeoutCheckCallback(void);
#endif // #ifdef STM32L4xx_HAL_UART_H

#endif /* A_OS_UART_ENABLED */
