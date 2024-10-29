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

#define	NO_SENTINEL	0

extern	void HAL_UART_RxTimeoutCheckCallback(void);

extern	void A_hw_uart_init(void);

#endif /* KERNEL_HWDEVICES_HW_UART_H_ */
