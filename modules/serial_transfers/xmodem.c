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
 * xmodem.c
 *
 *  Created on: Jan 31, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
//#include "../../kernel/kernel_opt.h"

#include "xmodem.h"
#include <string.h>

uint8_t 	xmodem_sm;
xmodem_t	xmodem_struct;

static uint8_t xmodem_calc_csum(uint8_t *buf)
{
uint16_t	i;
	xmodem_struct.calculated_checksum=0;
	xmodem_struct.packet_checksum=buf[XMODEM_CS];
	for(i=0;i<XMODEM_LEN;i++)
		xmodem_struct.calculated_checksum += buf[i+3];
	return ( xmodem_struct.calculated_checksum & 0xff ) - buf[XMODEM_CS];
}

uint8_t xmodem_line_parser(uint8_t *buf)
{
	if ( buf[0] == X_SOH)
	{
		xmodem_struct.data_len = XMODEM_LEN;
		if ( (buf[XMODEM_ADDR] + buf[XMODEM_ADDRI]) == 0xff)
		{
			xmodem_struct.addr = buf[XMODEM_ADDR];
			xmodem_struct.addri = buf[XMODEM_ADDRI];
			if ( xmodem_struct.data_len == XMODEM_LEN )
				xmodem_struct.cs = buf[XMODEM_CS];
			xmodem_struct.data_count += xmodem_struct.data_len;
			if ( xmodem_struct.data_count > xmodem_struct.requested_data_count )	// at 256K stop transfers
				return 1;
			memcpy(xmodem_struct.data_ptr,&buf[3],xmodem_struct.data_len);
			xmodem_struct.data_ptr += xmodem_struct.data_len;
			xmodem_struct.checksum = xmodem_calc_csum(buf);
			if ( xmodem_struct.checksum )
				return 2;
			return 0;
		}
	}
	if ( buf[0] == X_EOT)
	{
		return 1;
	}
	return 255;
}

uint8_t xmodem_process(uint32_t wakeup)
/*
 * Returns:
 * 0		:	processing
 * X_STX	:	polling host
 * X_EOT	:	all data received
 * X_NAK	:	line error
 * X_AK		:	line received
 */
{
uint8_t		ak_char=X_ACK, nak_char=X_NAK, ret_val;

	if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
	{
		if ( xmodem_struct.state == XMODEM_SEND_NAK )
		{
			xmodem_struct.data_ptr = xmodem_struct.requested_data_ptr;
			xmodem_struct.data_count = 0;
			uart_driver_send_buffer(xmodem_struct.uart_handle,&nak_char,1);
			ret_val = 0;
		}
		else
		{
			xmodem_struct.xtimeout--;
			if ( xmodem_struct.xtimeout == 0 )
			{
				xmodem_struct.xtimeout = XMODEM_TIMEOUT;
				xmodem_struct.state = XMODEM_SEND_NAK;
			}
		}
	}
	else if (( wakeup & (xmodem_struct.xmodem_wakeup_mask)) != 0)
	{
		xmodem_struct.state = XMODEM_DATA_PHASE;
		xmodem_struct.xtimeout = XMODEM_TIMEOUT;
		switch(xmodem_line_parser(xmodem_struct.rxbuf))
		{
		case 0 :
			xmodem_struct.received_bytes_count += XMODEM_LEN;
			if ( xmodem_struct.auto_send_ack == XMODEM_AUTOSEND_AK )
				uart_driver_send_buffer(xmodem_struct.uart_handle,&ak_char,1);
			ret_val = X_ACK;
			break;
		case 1 :
			if ( xmodem_struct.auto_send_ack == XMODEM_AUTOSEND_AK )
				uart_driver_send_buffer(xmodem_struct.uart_handle,&ak_char,1);
			xmodem_struct.state = XMODEM_SEND_NAK;
			ret_val = X_EOT;
			break;
		default:
			uart_driver_send_buffer(xmodem_struct.uart_handle,&nak_char,1);
			xmodem_struct.num_retries--;
			if ( xmodem_struct.num_retries == 0 )
			{
				xmodem_struct.num_retries = XMODEM_RETRIES;
				ret_val = X_NAK;
			}
			break;
		}
	}
	else
		ret_val = 0;
	return ret_val;
}

uint8_t xmodem_send_ack(void)
{
uint8_t		ak_char=X_ACK;
	return (uint8_t )uart_driver_send_buffer(xmodem_struct.uart_handle,&ak_char,1);
}

uint8_t xmodem_allocate_area(uint8_t *data_ptr,uint32_t max_data_count)
{
uint32_t	i_clear;
	xmodem_struct.data_ptr = xmodem_struct.requested_data_ptr = data_ptr;
	xmodem_struct.requested_data_count = max_data_count;
	xmodem_struct.data_count = 0;
	for(i_clear=0;i_clear<max_data_count;i_clear++)
		data_ptr[i_clear] = 0xff;
	return 0;
}

uint32_t xmodem_get_rxed_amount(void)
{
	return xmodem_struct.received_bytes_count;
}

void xmodem_init(uint8_t uart_handle,uint8_t *data_ptr,uint8_t *databuf_ptr,uint32_t max_data_count,uint8_t auto_send_ack,uint32_t wakeup_mask)
{
	xmodem_struct.uart_handle = uart_handle;
	xmodem_struct.xmodem_wakeup_mask = wakeup_mask;
	xmodem_allocate_area(data_ptr,max_data_count);
	xmodem_struct.state = XMODEM_SEND_NAK;
	xmodem_struct.xtimeout = XMODEM_TIMEOUT;
	xmodem_struct.rxbuf = databuf_ptr;
	xmodem_struct.received_bytes_count = 0;
	xmodem_struct.auto_send_ack = auto_send_ack;
	xmodem_struct.num_retries = XMODEM_RETRIES;
	uart_driver_receive_buffer((uint8_t )uart_handle,xmodem_struct.rxbuf,XMODEM_LINE_LEN);
}
