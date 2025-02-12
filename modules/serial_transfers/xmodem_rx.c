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



