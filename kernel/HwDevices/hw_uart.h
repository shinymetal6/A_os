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
 * hw_uart.h
 *
 *  Created on: Mar 1, 2024
 *      Author: fil
 */

#ifndef KERNEL_HWDEVICES_HW_UART_H_
#define KERNEL_HWDEVICES_HW_UART_H_

#define	HW_UART_OK		0
#define	HW_UART_ERROR	255

extern	uint32_t hw_send_uart(uint32_t uart,uint8_t *ptr,uint16_t len);
extern	uint32_t hw_send_uart_dma(uint32_t uart,uint8_t *ptr,uint16_t len);
extern	uint32_t hw_receive_uart(uint32_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint16_t timeout);
extern	uint32_t hw_receive_uart_sentinel(uint32_t uart,uint8_t *rx_buf,uint16_t rx_buf_max_len,uint8_t sentinel_start, uint8_t sentinel_end,uint16_t timeout);
extern	uint32_t hw_receive_uart_sentinel_clear(uint32_t uart);
extern	uint16_t hw_get_uart_receive_len(uint32_t uart);
extern	void HAL_UART_RxTimeoutCheckCallback(void);

extern	void A_hw_uart_init(void);


#endif /* KERNEL_HWDEVICES_HW_UART_H_ */
