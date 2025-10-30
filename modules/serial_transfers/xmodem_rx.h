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
 * xmodem_rx.h
 *
 *  Created on: Feb 12, 2025
 *      Author: fil
 */

#ifndef MODULES_SERIAL_TRANSFERS_XMODEM_RX_H_
#define MODULES_SERIAL_TRANSFERS_XMODEM_RX_H_

#define X_SOH	0x01
#define X_STX 	0x02
#define X_EOT	0x04
#define X_ACK	0x06
#define X_BEL	0x07
#define X_BS	0x08
#define X_NAK	0x15
#define X_CAN	0x18
#define X_DEL	0x7f

#define	XMODEM_LEN		128
#define	XMODEM_ADDR		1
#define	XMODEM_ADDRI	2
#define	XMODEM_CS		131
#define	XMODEM_LINE_LEN	XMODEM_LEN+4

typedef struct
{
	uint8_t		addr;
	uint8_t		addri;
	uint8_t		cs;
	uint8_t		checksum;
	uint8_t		calculated_checksum;
	uint8_t		*data_ptr;
	uint8_t		*requested_data_ptr;
	uint32_t	requested_data_count;
	uint32_t	received_bytes_count;
	uint32_t	session_received_bytes_count;
}xmodem_rx_t;

#define	XMODEM_IF_USB		0
#define	XMODEM_IF_UART		1

extern	void 		xmodem_rx_init(uint8_t *dest_data_ptr,uint32_t max_data_count );
extern	void 		xmodem_rx_set_data_area(uint8_t *dest_data_ptr,uint32_t max_data_count );

extern	uint8_t 	xmodem_rx_process(uint32_t wakeup);
extern	uint32_t 	xmodem_rx_get_rxed_amount(void);
extern	uint8_t 	xmodem_rx_send_ack(void);
extern	uint8_t 	xmodem_rx_line_parser(uint8_t *buf);
extern	uint8_t 	xmodem_uart_data_process(UART_Drv_TypeDef *uart_drv,uint8_t mode,uint8_t *uart_rx_buffer);
extern	uint8_t 	xmodem_usb_data_process(USB_Drv_TypeDef *usb_drv,uint8_t mode,uint8_t *usb_rx_buffer);
extern	uint8_t 	xmodem_data_process(uint32_t *driver,uint8_t mode,uint8_t type,uint8_t *rx_buffer);


#endif /* MODULES_SERIAL_TRANSFERS_XMODEM_RX_H_ */
