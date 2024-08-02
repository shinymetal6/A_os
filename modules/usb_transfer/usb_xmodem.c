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
 * usb_xmodem.c
 *
 *  Created on: Jun 6, 2024
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
#include "../../kernel/kernel_opt.h"

#ifdef USB_XMODEM_ENABLE

#include "usb_xmodem.h"
#include <string.h>

usb_xmodem_t	usb_xmodem;
uint16_t		usb_xmodem_pkt_len;
uint8_t			usb_ak_char=USB_X_ACK, usb_nak_char=USB_X_NAK;
extern	void task_delay(uint32_t tick_count);

static uint8_t usb_xmodem_calc_csum(uint8_t *buf)
{
uint16_t	calc_csum=0,i;
	for(i=0;i<USB_XMODEM_LEN;i++)
		calc_csum += buf[i+3];
	return ( calc_csum & 0xff ) - buf[USB_XMODEM_LEN+3];
}

static uint8_t usb_xmodem_line_parser(uint8_t *buf)
{
	if ( buf[0] == USB_X_SOH)
	{
		usb_xmodem.data_len = USB_XMODEM_LEN;
		if ( (buf[USB_XMODEM_ADDR] + buf[USB_XMODEM_ADDRI]) == 0xff)
		{
			usb_xmodem.addr = buf[USB_XMODEM_ADDR];
			usb_xmodem.addri = buf[USB_XMODEM_ADDRI];
			if ( usb_xmodem.data_len == USB_XMODEM_LEN )
				usb_xmodem.cs = buf[USB_XMODEM_CS];
			if ( buf[0] == USB_X_SOH)
				return usb_xmodem_calc_csum(buf);
		}
	}
	return 1;
}

uint8_t usb_xmodem_process(uint32_t wakeup, uint8_t auto_send_ack)
{
uint8_t	i;
	if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
	{
		if ( usb_xmodem.xtimeout > 10 )
		{
			usb_xmodem.state = USB_XMODEM_ASKFILE_PHASE;
			hw_send_usb(&usb_nak_char,1);
			usb_xmodem.xtimeout = 0;
			usb_xmodem.received_bytes_count = 0;
			usb_xmodem.usb_xmodem_line_cntr = 0;
			usb_xmodem.received_pkts = 0;
		}
		else
		{
			usb_xmodem.xtimeout++;
		}
	}
	if (( wakeup & WAKEUP_FROM_USB_DEVICE_IRQ) == WAKEUP_FROM_USB_DEVICE_IRQ)
	{
		usb_xmodem.state = USB_XMODEM_DATA_PHASE;
		usb_xmodem.xtimeout = 0;
		usb_xmodem_pkt_len = hw_UsbGetRXLen();

		for(i=0;i<usb_xmodem_pkt_len;i++,usb_xmodem.usb_xmodem_line_cntr++,usb_xmodem.received_bytes_count++)
			usb_xmodem.usb_xmodem_line[usb_xmodem.usb_xmodem_line_cntr] = usb_xmodem.rxbuf[i];

		if ( usb_xmodem_pkt_len == USB_XMODEM_LASTPLEN )
		{
			if ( usb_xmodem_line_parser(usb_xmodem.usb_xmodem_line) == 0 )
			{
				memcpy(usb_xmodem.process_rxbuf_ptr,&usb_xmodem.usb_xmodem_line[USB_XMODEM_DSTART],USB_XMODEM_LEN);
				if ( usb_xmodem.mode == USB_XMODEM_MODE_ALL_ALLOCATED)
				{
					memcpy(&usb_xmodem.usb_xmodem_last_line[0],&usb_xmodem.usb_xmodem_line[USB_XMODEM_DSTART],USB_XMODEM_LEN);
					usb_xmodem.process_rxbuf_ptr += USB_XMODEM_LEN;
				}
				usb_xmodem.received_pkts ++;
				usb_xmodem.current_pktrx_result = USB_XMODEM_PKT_CORRECT;
				if (auto_send_ack == USB_XMODEM_AUTOSEND_AK)
					hw_send_usb(&usb_ak_char,1);
			}
			else
			{
				usb_xmodem.nackd_pkts++;
				hw_send_usb(&usb_nak_char,1);
				usb_xmodem.current_pktrx_result = USB_XMODEM_PKT_ERROR;
			}
			usb_xmodem.usb_xmodem_line_cntr = 0;
			usb_xmodem.state = USB_XMODEM_END_DATA_PHASE;
		}
		if ( usb_xmodem_pkt_len == 1 )
		{
			if (auto_send_ack == USB_XMODEM_AUTOSEND_AK)
				hw_send_usb(&usb_ak_char,1);
			usb_xmodem.usb_xmodem_line_cntr = 0;
			usb_xmodem.state = USB_XMODEM_END_RECEIVE;
		}
	}
	return usb_xmodem.state;
}

uint8_t usb_xmodem_send_ack(void)
{
	return (uint8_t )hw_send_usb(&usb_ak_char,1);
}

uint8_t usb_xmodem_send_nack(void)
{
	return (uint8_t )hw_send_usb(&usb_nak_char,1);
}

uint8_t usb_xmodem_get_pktrx_result(void)
{
	return usb_xmodem.current_pktrx_result;
}

uint8_t *usb_xmodem_get_last_line_ptr(void)
{
	return usb_xmodem.usb_xmodem_last_line;
}

void usb_xmodem_init(uint8_t *data_ptr, uint8_t mode)
{
	allocate_hw(HW_USB_DEVICE,0);
	usb_xmodem.usb_xmodem_line_cntr = 0;
	usb_xmodem.nackd_pkts = 0;
	if ( mode != 0)
		usb_xmodem.mode = USB_XMODEM_MODE_ALL_ALLOCATED;
	else
		usb_xmodem.mode = USB_XMODEM_MODE_SINGLELINE;
	usb_xmodem.process_rxbuf_ptr = data_ptr;
	usb_xmodem.state = USB_XMODEM_ASKFILE_PHASE;
	hw_set_usb_rx_buffer(usb_xmodem.rxbuf);
}

#endif
