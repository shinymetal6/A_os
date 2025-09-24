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
 * xmodem_rx.c
 *
 *  Created on: Feb 12, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/A.h"
#ifdef	XMODEM_ENABLE

#include "../../kernel/system_default.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/kernel_opt.h"

#include "xmodem_rx.h"
#include <string.h>

xmodem_rx_t	xmodem_rx_struct;

ITCM_AREA_CODE	static uint8_t xmodem_rx_allocate_area(uint8_t *data_ptr,uint32_t max_data_count)
{
uint32_t	i;
	xmodem_rx_struct.data_ptr = xmodem_rx_struct.requested_data_ptr = data_ptr;
	xmodem_rx_struct.requested_data_count = max_data_count;
	for(i=0;i<max_data_count;i++)
		data_ptr[i] = 0xff;
	return 0;
}

ITCM_AREA_CODE	static uint8_t xmodem_rx_calc_csum(uint8_t *buf)
{
uint16_t	calc_csum=0,i;
	for(i=0;i<XMODEM_LEN;i++)
		calc_csum += buf[i+3];
	return calc_csum & 0xff;
}

ITCM_AREA_CODE	uint8_t xmodem_rx_line_parser(uint8_t *buf)
{
	if ( buf[0] == X_EOT)
	{
		xmodem_rx_struct.session_received_bytes_count = xmodem_rx_struct.received_bytes_count;
		xmodem_rx_struct.received_bytes_count = 0;
		xmodem_rx_struct.data_ptr = xmodem_rx_struct.requested_data_ptr;
		return X_EOT;
	}
	if ( buf[0] == X_SOH)
	{
		if ( (buf[XMODEM_ADDR] + buf[XMODEM_ADDRI]) != 0xff)
			return X_NAK;
		xmodem_rx_struct.addr = buf[XMODEM_ADDR];
		xmodem_rx_struct.addri = buf[XMODEM_ADDRI];
		xmodem_rx_struct.cs = buf[XMODEM_CS];
		xmodem_rx_struct.calculated_checksum = xmodem_rx_calc_csum(buf);

		if ( xmodem_rx_struct.calculated_checksum == xmodem_rx_struct.cs)
		{
			memcpy(xmodem_rx_struct.data_ptr,&buf[3],XMODEM_LEN);
			xmodem_rx_struct.data_ptr += XMODEM_LEN;
			xmodem_rx_struct.received_bytes_count += XMODEM_LEN;
			if ( xmodem_rx_struct.received_bytes_count >= xmodem_rx_struct.requested_data_count)
				return X_NAK;
			return X_ACK;
		}
		return X_NAK;
	}
	return X_DEL;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_get_rxed_amount(void)
{
	return xmodem_rx_struct.session_received_bytes_count;
}

ITCM_AREA_CODE	void xmodem_rx_init(uint8_t *dest_data_ptr,uint32_t max_data_count )
{
	xmodem_rx_allocate_area(dest_data_ptr,max_data_count);
}

ITCM_AREA_CODE	void xmodem_rx_set_data_area(uint8_t *dest_data_ptr,uint32_t max_data_count )
{
	xmodem_rx_allocate_area(dest_data_ptr,max_data_count);
}
uint8_t		xnak=X_NAK,xack=X_ACK;
ITCM_AREA_CODE	uint8_t xmodem_uart_data_process(uint8_t mode,uint32_t uart_driver_handle,uint8_t *uart_rx_buffer)
{
uint8_t		xmodem_rx_uart_reply,rxlen;
	if ( mode == 1 )
	{
		uart_send(uart_driver_handle,&xnak,1);
		return 0;
	}
	if ( mode == 0 )
	{
		rxlen = uart_get_rxlen(uart_driver_handle);
		if ( rxlen > 1 )
		{
			if (( uart_rx_buffer[0] == X_SOH ) || ( uart_rx_buffer[0] == X_EOT ))
				xmodem_rx_uart_reply = xmodem_rx_line_parser(uart_rx_buffer);
			else
				xmodem_rx_uart_reply = xmodem_rx_line_parser(&uart_rx_buffer[1]);
			uart_restart_DMA_on_RX(uart_driver_handle);
			switch(xmodem_rx_uart_reply)
			{
			case	X_NAK:
				uart_send(uart_driver_handle,&xnak,1);
				break;
			case	X_EOT:
				uart_send(uart_driver_handle,&xack,1);
				break;
			case	X_ACK:
				uart_send(uart_driver_handle,&xack,1);
				break;
			default:
				uart_send(uart_driver_handle,&xnak,1);
				return 0xff;
				break;
			}
			return xmodem_rx_uart_reply;
		}
	}
	return 0xff;
}

ITCM_AREA_CODE	uint8_t xmodem_usb_data_process(uint8_t mode,uint32_t usb_handle,uint8_t *usb_rx_buffer)
{
uint8_t		xmodem_usb_uart_reply;
	if ( mode )
	{
		usb_send(usb_handle,&xnak,1);
		return 0;
	}
	else
	{
		xmodem_usb_uart_reply = xmodem_rx_line_parser(usb_rx_buffer);
		switch(xmodem_usb_uart_reply)
		{
		case	X_NAK:
			usb_send(usb_handle,&xnak,1);
			break;
		case	X_EOT:
			usb_send(usb_handle,&xack,1);
			break;
		case	X_ACK:
			usb_send(usb_handle,&xack,1);
			break;
		default:
			usb_send(usb_handle,&xnak,1);
			return 0xff;
			break;
		}
		return xmodem_usb_uart_reply;
	}
	return 0xff;
}

ITCM_AREA_CODE	uint8_t xmodem_data_process(uint8_t mode,uint8_t type,uint32_t handle,uint8_t *rx_buffer)
{
	if ( type == XMODEM_IF_USB)
		return xmodem_usb_data_process(mode,handle,rx_buffer);
	if ( type == XMODEM_IF_UART)
		return xmodem_uart_data_process(mode,handle,rx_buffer);
	return 1;
}

#endif // #ifdef	XMODEM_ENABLE




