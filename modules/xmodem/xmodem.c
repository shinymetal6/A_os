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
#include "../../kernel/scheduler.h"

#ifdef XMODEM_ENABLE

#include "xmodem.h"
#include <string.h>

uint8_t 	xmodem_sm;
xmodem_t	xmodem_struct;
uint8_t		xmodem_line[1024];

static uint8_t xmodem_calc_csum(uint8_t *buf)
{
uint16_t	calc_csum=0,i;
	for(i=0;i<XMODEM_LEN;i++)
		calc_csum += buf[i+3];
	return ( calc_csum & 0xff ) - buf[XMODEM_LEN+3];
}

static uint8_t xmodem_calc_crc(uint8_t *buf)
{
	return 0;
}

uint8_t xmodem_line_parser(uint8_t *buf)
{
	if ( buf[0] == X_SOH)
		xmodem_struct.data_len = XMODEM_LEN;
	else if ( buf[0] == X_STX)
		xmodem_struct.data_len = XMODEM1K_LEN;
	else
		return 1;
	if ( (buf[XMODEM_ADDR] + buf[XMODEM_ADDRI]) != 0xff)
		return 1;
	xmodem_struct.addr = buf[XMODEM_ADDR];
	xmodem_struct.addri = buf[XMODEM_ADDRI];
	if ( xmodem_struct.data_len == XMODEM_LEN )
		xmodem_struct.cs = buf[XMODEM_CS];
	xmodem_struct.data_count += xmodem_struct.data_len;
	if ( xmodem_struct.data_count > xmodem_struct.requested_data_count )	// at 256K stop transfers
		return 1;
	memcpy(xmodem_struct.data_ptr,&buf[3],xmodem_struct.data_len);
	xmodem_struct.data_ptr += xmodem_struct.data_len;
	if ( buf[0] == X_SOH)
		return xmodem_calc_csum(buf);
	else if ( buf[0] == X_STX)
		return xmodem_calc_crc(buf);
	else
		return 1;
}

extern	UART_HandleTypeDef huart3;
uint8_t ret;
uint16_t	pkt_len;
uint32_t	xmodem_wakeup_mask = 0;

uint8_t xmodem_process(uint32_t wakeup)
{
uint8_t		ak_char=X_ACK, nak_char=X_NAK, ret_val = 0;

	if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
	{
		if ( xmodem_struct.state == XMODEM_SEND_NAK )
		{
			xmodem_struct.data_ptr = xmodem_struct.requested_data_ptr;
			xmodem_struct.data_count = 0;
			hw_send_uart(xmodem_struct.uart,&nak_char,1);
		}
		else
		{
			xmodem_struct.xtimeout++;
			if ( xmodem_struct.xtimeout > 5 )
			{
				hw_send_uart(xmodem_struct.uart,&nak_char,1);
				xmodem_struct.xtimeout = 0;
				xmodem_struct.state = XMODEM_SEND_NAK;
			}
		}
	}
	if (( wakeup & (xmodem_wakeup_mask)) != 0)
	{
		xmodem_struct.state = XMODEM_DATA_PHASE;
		pkt_len = hw_get_uart_receive_len(xmodem_struct.uart);
		if ( pkt_len == 1 )
		{
			if ( xmodem_struct.rxbuf[0] == X_EOT )
			{
				hw_send_uart(xmodem_struct.uart,&ak_char,1);
				xmodem_struct.state = XMODEM_SEND_NAK;
				xmodem_struct.xtimeout = 0;
				xmodem_struct.data_ptr = xmodem_struct.requested_data_ptr;
				xmodem_struct.data_count = 0;
				ret_val = X_EOT;
			}
		}
		else if ( pkt_len == 132 )
		{
			if ( xmodem_line_parser(xmodem_struct.rxbuf))
			{
				hw_send_uart(xmodem_struct.uart,&nak_char,1);
			}
			else
			{
				hw_send_uart(xmodem_struct.uart,&ak_char,1);
			}
			xmodem_struct.xtimeout = 0;
		}
		else
		{
			xmodem_struct.xtimeout = 0;
			hw_send_uart(xmodem_struct.uart,&nak_char,1);
		}
	}
	return ret_val;
}

uint8_t xmodem_allocate_area(uint8_t *data_ptr)
{
	xmodem_struct.data_ptr = xmodem_struct.requested_data_ptr = data_ptr;
	xmodem_struct.data_count = 0;
	return 0;
}

void xmodem_init(uint32_t uart,uint8_t *data_ptr,uint32_t max_data_count)
{
#ifdef	A_HAS_UART1
		xmodem_wakeup_mask |= WAKEUP_FROM_UART1_IRQ;
#endif
#ifdef	A_HAS_UART2
		xmodem_wakeup_mask |= WAKEUP_FROM_UART2_IRQ;
#endif
#ifdef	A_HAS_UART3
		xmodem_wakeup_mask |= WAKEUP_FROM_UART3_IRQ;
#endif
#ifdef	A_HAS_UART4
		xmodem_wakeup_mask |= WAKEUP_FROM_UART4_IRQ;
#endif
#ifdef	A_HAS_UART5
		xmodem_wakeup_mask |= WAKEUP_FROM_UART5_IRQ;
#endif
#ifdef	A_HAS_UART6
		xmodem_wakeup_mask |= WAKEUP_FROM_UART6_IRQ;
#endif
#ifdef	A_HAS_UART7
		xmodem_wakeup_mask |= WAKEUP_FROM_UART7_IRQ;
#endif
	hw_receive_uart_sentinel(uart,xmodem_struct.rxbuf,XMODEM_LEN+4,X_SOH, X_EOT,XMODEM_TIMEOUT);

	xmodem_allocate_area(data_ptr);
	xmodem_struct.uart = uart;
	xmodem_struct.state = XMODEM_SEND_NAK;
	xmodem_struct.xtimeout = 0;
	xmodem_struct.requested_data_count = max_data_count;
}

#endif
