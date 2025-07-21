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
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/kernel_opt.h"

#include "xmodem_rx.h"
#include <string.h>

xmodem_rx_t	xmodem_rx_struct;
XMODEM_Mod_TypeDef	*xmodem;

ITCM_AREA_CODE	static uint32_t x_rx_allocate_area(uint8_t *data_ptr,uint32_t max_data_count)
{
uint32_t	i;
	xmodem_rx_struct.data_ptr = xmodem_rx_struct.requested_data_ptr = data_ptr;
	xmodem_rx_struct.requested_data_count = max_data_count;
	for(i=0;i<max_data_count;i++)
		data_ptr[i] = 0xff;
	return 0;
}

ITCM_AREA_CODE	static uint8_t x_rx_calc_csum(uint8_t *buf)
{
uint16_t	calc_csum=0,i;
	for(i=0;i<XMODEM_LEN;i++)
		calc_csum += buf[i+3];
	return calc_csum & 0xff;
}

ITCM_AREA_CODE	static uint32_t x_rx_line_parser(uint8_t *buf)
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
		xmodem_rx_struct.calculated_checksum = x_rx_calc_csum(buf);

		if ( xmodem_rx_struct.calculated_checksum == xmodem_rx_struct.cs)
		{
			memcpy(xmodem_rx_struct.data_ptr,&buf[3],XMODEM_LEN);
			xmodem_rx_struct.data_ptr += XMODEM_LEN;
			xmodem_rx_struct.received_bytes_count += XMODEM_LEN;
			if ( xmodem_rx_struct.received_bytes_count >= xmodem_rx_struct.requested_data_count)
				return X_DEL;
			return X_ACK;
		}
		return X_NAK;
	}
	return X_DEL;
}

ITCM_AREA_CODE	static uint32_t x_rx_get_rxed_amount(void)
{
	return xmodem_rx_struct.session_received_bytes_count;
}

ITCM_AREA_CODE	static uint32_t x_rx_send_ack(void)
{
	if ( xmodem->xmodem_dev == XMODEM_DEV_USB)
		usb_send(xmodem->xmodem_dev_handle,&xmodem->ack,1);
	else
		uart_send(xmodem->xmodem_dev_handle,&xmodem->ack,1);
	return 0;
}

ITCM_AREA_CODE	static uint32_t x_rx_send_nack(void)
{
	if ( xmodem->xmodem_dev == XMODEM_DEV_USB)
		usb_send(xmodem->xmodem_dev_handle,&xmodem->nak,1);
	else
		uart_send(xmodem->xmodem_dev_handle,&xmodem->nak,1);
	return 0;
}

ITCM_AREA_CODE	static uint32_t x_rx_init(uint8_t *dest_data_ptr,uint32_t max_data_count )
{
	xmodem->xmodem_flags &= ~XMODEM_ENABLE_POLL;
	xmodem->xmodem_to = 0;
	return x_rx_allocate_area(dest_data_ptr,max_data_count);
}

ITCM_AREA_CODE	static uint32_t x_rx_set_data_area(uint8_t *dest_data_ptr,uint32_t max_data_count )
{
	return x_rx_allocate_area(dest_data_ptr,max_data_count);
}

ITCM_AREA_CODE	static uint32_t x_enable_poll(uint32_t enable )
{
	if ( enable )
		xmodem->xmodem_flags |= XMODEM_ENABLE_POLL;
	else
		xmodem->xmodem_flags &= ~XMODEM_ENABLE_POLL;
	return (uint32_t )xmodem->xmodem_flags;
}

ITCM_AREA_CODE	static uint32_t x_rx(uint32_t wakeup )
{
	if ( wakeup == XMODEM_WAKEUP_TIMER)
	{
		if ( (xmodem->xmodem_flags & XMODEM_ENABLE_POLL ) == XMODEM_ENABLE_POLL)
		{
			if ( xmodem->xmodem_to >= xmodem->xmodem_rx_to)
			{
				x_rx_send_nack();
				xmodem->xmodem_to = 0;
			}
			else
				xmodem->xmodem_to ++;
		}
	}
	if ( wakeup == XMODEM_WAKEUP_DEVICE)
	{
		xmodem->xmodem_flags &= ~XMODEM_ENABLE_POLL;
		xmodem->xmodem_rx_reply = x_rx_line_parser(xmodem->xmodem_buffer);
		switch(xmodem->xmodem_rx_reply)
		{
		case	X_NAK:
			x_rx_send_nack();
			break;
		case	X_EOT:
			x_rx_send_ack();
			xmodem->xmodem_flags |= XMODEM_ENABLE_POLL;
			xmodem->xmodem_to = 0;
			break;
		case	X_ACK:
			x_rx_send_ack();
			break;
		case	X_DEL:
			x_rx_send_nack();
			break;
		default:
			x_rx_send_nack();
			break;
		}
		return xmodem->xmodem_rx_reply;
	}
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_register(XMODEM_Mod_TypeDef *private_data)
{
	__disable_irq();
	if ( private_data->flags != 0 )
	{
		if ( private_data->wakeup_id == 0)
		{
			__enable_irq();
			return DRIVER_REQUEST_FAILED;
		}
	}
	if ( private_data->xmodem_buffer == NULL )
		return DRIVER_REQUEST_FAILED;

	private_data->rx_init = x_rx_init;
	private_data->rx_set_data_area = x_rx_set_data_area;
	private_data->rx_get_rxed_amount = x_rx_get_rxed_amount;
	private_data->rx_line_parser = x_rx_line_parser;
	private_data->rx_send_ack = x_rx_send_ack;
	private_data->rx_send_nack = x_rx_send_nack;
	private_data->rx_enable_poll = x_enable_poll;
	private_data->rx = x_rx;
	private_data->nak=X_NAK;
	private_data->ack=X_ACK;
	x_rx_allocate_area((uint8_t *)private_data->xmodem_rx_data_area,private_data->xmodem_rx_data_len);
	xmodem = private_data;
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_init(uint8_t *dest_data_ptr,uint32_t max_data_count )
{
	if ( xmodem->rx_init != NULL )
		return xmodem->rx_init(dest_data_ptr,max_data_count);
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_enable_poll(uint32_t enable )
{
	if ( xmodem->rx_enable_poll != NULL )
		return xmodem->rx_enable_poll(enable);
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx(uint32_t wakeup )
{
	if ( xmodem->rx != NULL )
		return xmodem->rx(wakeup);
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_set_data_area(uint8_t *dest_data_ptr,uint32_t max_data_count )
{
	if ( xmodem->rx_set_data_area != NULL )
		return xmodem->rx_set_data_area(dest_data_ptr,max_data_count);
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_send_nack(void)
{
	if ( xmodem->rx_send_nack != NULL )
		return xmodem->rx_send_nack();
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_send_ack(void)
{
	if ( xmodem->rx_send_ack != NULL )
		return xmodem->rx_send_ack();
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_get_rxed_amount(void)
{
	if ( xmodem->rx_get_rxed_amount != NULL )
		return xmodem->rx_get_rxed_amount();
	return 0;
}

ITCM_AREA_CODE	uint32_t xmodem_rx_line_parser(uint8_t *buf)
{
	if ( xmodem->rx_line_parser != NULL )
		return xmodem->rx_line_parser(buf);
	return 0;
}
