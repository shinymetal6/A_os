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
 * xmodem.h
 *
 *  Created on: Jan 31, 2024
 *      Author: fil
 */

#ifndef MODULES_XMODEM_XMODEM_H_
#define MODULES_XMODEM_XMODEM_H_

#ifdef XMODEM_ENABLE

#define X_SOH	0x01
#define X_STX 	0x02
#define X_EOT	0x04
#define X_ACK	0x06
#define X_NAK	0x15
#define X_CAN	0x18
#define X_C		'C'  /* notify the host we can use CRC16. */

#define IDLE			0
#define	RECV_SOH		1
#define	RECV_ADDR		2
#define	RECV_ADDRI		3
#define	DATA_PHASE		4
#define	CS_PHASE		5
#define	CRCH_PHASE		6
#define	CRCL_PHASE		7
#define	RECV_LOOP		8
#define	RECV_EOT		9

#define	XMODEM_LEN		128
#define	XMODEM_ADDR		1
#define	XMODEM_ADDRI	2
#define	XMODEM_CS		131
#define	XMODEM_LINE_LEN	XMODEM_LEN+4

#define	XMODEM_SEND_NAK			0
#define	XMODEM_DATA_PHASE		1
#define	XMODEM_TIMEOUT			500
#define	XMODEM_MAX_EOT_PKTLEN	32
#define	XMODEM_AUTOSEND_AK		1
#define	XMODEM_USERSEND_AK		0

typedef struct
{
	uint16_t	data_len;
	uint8_t		addr;
	uint8_t		addri;
	uint8_t		cs;
	uint8_t		crch;
	uint8_t		crcl;
	uint8_t		state;
	uint8_t		xtimeout;
	uint32_t	uart;
	uint8_t		rxbuf[XMODEM_LEN+4];
	uint8_t		last_rxbuf[XMODEM_LEN+4];
	uint8_t		*data_ptr;
	uint8_t		*requested_data_ptr;
	uint32_t	requested_data_count;
	uint32_t	data_count;
	uint32_t	received_bytes_count;
	uint32_t	last_received_bytes_count;
}xmodem_t;

extern	void 		xmodem_init(uint32_t uart,uint8_t *data_ptr,uint32_t max_data_count);
extern	uint8_t 	xmodem_process(uint32_t wakeup, uint8_t auto_send_ack);
extern	uint32_t 	xmodem_get_rxed_amount(void);
extern	uint8_t 	*xmodem_get_rxed_line(void);
extern	uint8_t 	xmodem_send_ack(void);

#endif // #ifdef XMODEM_ENABLE

#endif /* MODULES_XMODEM_XMODEM_H_ */
