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

static uint8_t xmodem_allocate_area(uint8_t *data_ptr,uint32_t max_data_count)
{
uint32_t	i_clear;
	xmodem_struct.data_ptr = xmodem_struct.requested_data_ptr = data_ptr;
	xmodem_struct.requested_data_count = max_data_count;
	xmodem_struct.data_count = 0;
	for(i_clear=0;i_clear<max_data_count;i_clear++)
		data_ptr[i_clear] = 0xff;
	return 0;
}

static uint8_t xmodem_calc_csum(uint8_t *buf)
{
uint16_t	calc_csum=0,i;
	for(i=0;i<XMODEM_LEN;i++)
		calc_csum += buf[i+3];
	return calc_csum & 0xff;
}

uint8_t xmodem_line_parser(uint8_t *buf)
{
	if ( buf[0] == X_EOT)
		return X_ACK;
	if ( buf[0] == X_SOH)
	{
		xmodem_struct.data_len = XMODEM_LEN;
		if ( (buf[XMODEM_ADDR] + buf[XMODEM_ADDRI]) != 0xff)
			return X_NAK;
		xmodem_struct.addr = buf[XMODEM_ADDR];
		xmodem_struct.addri = buf[XMODEM_ADDRI];
		if ( xmodem_struct.data_len == XMODEM_LEN )
			xmodem_struct.cs = buf[XMODEM_CS];
		xmodem_struct.calculated_checksum = xmodem_calc_csum(buf);

		if ( xmodem_struct.calculated_checksum == xmodem_struct.cs)
		{
			memcpy(xmodem_struct.data_ptr,&buf[3],xmodem_struct.data_len);
			xmodem_struct.data_ptr += xmodem_struct.data_len;
			return X_ACK;
		}
		return X_NAK;
	}
	return X_DEL;
}

uint32_t xmodem_get_rxed_amount(void)
{
	return xmodem_struct.received_bytes_count;
}

void xmodem_init(uint8_t *dest_data_ptr,uint32_t max_data_count )
{
	xmodem_allocate_area(dest_data_ptr,max_data_count);
}

void xmodem_set_data_area(uint8_t *dest_data_ptr,uint32_t max_data_count )
{
	xmodem_allocate_area(dest_data_ptr,max_data_count);
}

