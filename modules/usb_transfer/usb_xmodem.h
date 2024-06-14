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
 * usb_xmodem.h
 *
 *  Created on: Jun 6, 2024
 *      Author: fil
 */

#ifndef MODULES_USB_TRANSFER_USB_XMODEM_H_
#define MODULES_USB_TRANSFER_USB_XMODEM_H_

#define	USB_XMODEM_USBLEN	64
#define	USB_XMODEM_LEN		128
#define	USB_XMODEM_ADDR		1
#define	USB_XMODEM_ADDRI	2
#define	USB_XMODEM_DSTART	3
#define	USB_XMODEM_LASTPLEN	4
#define	USB_XMODEM_CS		131
#define	USB_XMODEM_LINE_LEN	(USB_XMODEM_CS+1)
#define	USB_XMODEM_REM		(USB_XMODEM_LINE_LEN-(2*USB_XMODEM_USBLEN))

#define USB_X_SOH	0x01
#define USB_X_STX 	0x02
#define USB_X_EOT	0x04
#define USB_X_ACK	0x06
#define USB_X_NAK	0x15
#define USB_X_CAN	0x18
#define USB_X_C		'C'  /* notify the host we can use CRC16. */

typedef struct
{
	uint16_t	data_len;
	uint8_t		addr;
	uint8_t		addri;
	uint8_t		cs;
	uint8_t		crch;
	uint8_t		crcl;
	uint8_t		state;
	uint8_t		mode;
	uint8_t		xtimeout;
	uint8_t		rxbuf[USB_XMODEM_USBLEN];
	uint8_t		rxbuf_cntr;
	uint8_t		*process_rxbuf_ptr;
	uint8_t		usb_xmodem_line[USB_XMODEM_LINE_LEN];
	uint8_t		usb_xmodem_line_cntr;
	uint8_t		usb_xmodem_last_line[USB_XMODEM_LEN];
	uint32_t	received_bytes_count;
	uint32_t	received_pkts;
	uint32_t	nackd_pkts;
	uint8_t		current_pktrx_result;
}usb_xmodem_t;

#define	USB_XMODEM_ASKFILE_PHASE	0
#define	USB_XMODEM_DATA_PHASE		1
#define	USB_XMODEM_END_DATA_PHASE	2
#define	USB_XMODEM_END_RECEIVE		3

#define	USB_XMODEM_PKT_CORRECT		0
#define	USB_XMODEM_PKT_ERROR		1

#define	USB_XMODEM_TIMEOUT			500
#define	USB_XMODEM_MAX_EOT_PKTLEN	32
#define	USB_XMODEM_AUTOSEND_AK		1
#define	USB_XMODEM_USERSEND_AK		0

#define	USB_XMODEM_MODE_SINGLELINE		0
#define	USB_XMODEM_MODE_ALL_ALLOCATED	1

extern	void usb_xmodem_init(uint8_t *data_ptr, uint8_t mode);
extern	uint8_t usb_xmodem_process(uint32_t wakeup, uint8_t auto_send_ack);
extern	uint8_t usb_xmodem_send_ack(void);
extern	uint8_t usb_xmodem_send_nack(void);
extern	uint8_t usb_xmodem_get_pktrx_result(void);
extern	uint8_t *usb_xmodem_get_last_line_ptr(void);

#endif /* MODULES_USB_TRANSFER_USB_XMODEM_H_ */
