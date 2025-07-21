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

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	uint8_t				nak;
	uint8_t				ack;
	uint8_t				xmodem_flags;
	uint8_t				xmodem_to;
	uint8_t				xmodem_rx_to;
	uint8_t				xmodem_rx_reply;
	uint8_t				*xmodem_buffer;
	uint32_t 			xmodem_dev;
	uint32_t 			xmodem_dev_handle;
	uint32_t 			xmodem_rx_data_area;
	uint32_t 			xmodem_rx_data_len;
	uint32_t			(*rx_init)  (uint8_t *dest_data_ptr,uint32_t max_data_count);
	uint32_t			(*rx_set_data_area)  (uint8_t *dest_data_ptr,uint32_t max_data_count );
	uint32_t			(*rx_get_rxed_amount)  (void);
	uint32_t			(*rx_line_parser)  (uint8_t *buf);
	uint32_t			(*rx_enable_poll)  (uint32_t enable);
	uint32_t			(*rx_send_ack)  (void);
	uint32_t			(*rx_send_nack)  (void);
	uint32_t			(*rx)  (uint32_t wakeup);
	uint32_t 			wakeup_id;
}XMODEM_Mod_TypeDef;
/* xmodem_flags */
#define	XMODEM_ENABLE_POLL	0x80
#define	XMODEM_DISABLE_POLL	0x00
#define	XMODEM_RXED_NACK	0x02
#define	XMODEM_RXED_ACK		0x01
/* xmodem_dev */
#define	XMODEM_DEV_USB		0x01
#define	XMODEM_DEV_UART		0x02

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

#define	XMODEM_WAKEUP_TIMER		0
#define	XMODEM_WAKEUP_DEVICE	1

extern	uint32_t xmodem_rx_register(XMODEM_Mod_TypeDef *private_data);
extern	uint32_t xmodem_rx_init(uint8_t *dest_data_ptr,uint32_t max_data_count );
extern	uint32_t xmodem_rx_enable_poll(uint32_t enable );
extern	uint32_t xmodem_rx(uint32_t wakeup );

#endif /* MODULES_SERIAL_TRANSFERS_XMODEM_RX_H_ */
