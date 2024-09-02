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
 * ihex.c
 *
 *  Created on: Aug 8, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../kernel/kernel_opt.h"

#ifdef INTEL_HEX_DECODER_ENABLE

#include "ihex.h"
#include <string.h>

SYSTEM_RAM	IHex_struct_t	IHex;

uint8_t ihex_byte(uint8_t byte1 , uint8_t byte2)
{
uint8_t	packed_byte;
	if ( byte1 <= '9' )
		packed_byte = (byte1 - '0') << 4;
	else
		packed_byte = ((byte1 - 'A')+10) << 4;
	if ( byte2 <= '9' )
		packed_byte |= (byte2 - '0');
	else
		packed_byte |= ((byte2 - 'A')+10);
	return packed_byte;
}

uint8_t	*get_ihex_data_ptr(void)
{
	return IHex.ihex_line;
}

uint32_t get_ihex_address(void)
{
	return IHex.address;
}

uint32_t ihex_get_data_len(uint8_t *data_ptr)
{
	return ((data_ptr[1] & 0x0f)<<4 ) | (data_ptr[2] & 0x0f);
}

uint32_t ihex_get_crlflen(uint8_t *data_ptr)
{
uint32_t	i;
	for(i=0;i<256;i++)
	{
		if (( data_ptr[i] == '\r') && ( data_ptr[i+1] == '\n'))
			return i+2;
		if (( data_ptr[i] == '\n') && ( data_ptr[i+1] == '\r'))
			return i+2;
	}
	return 0;
}

uint32_t ihex_check_line(uint8_t *ihex_buf , uint32_t ihex_len)
{
uint32_t	check=0 , i;

	for(i=1;i<ihex_len;i+=2)
		check += ihex_byte(ihex_buf[i] , ihex_buf[i+1]);
	return ihex_byte(ihex_buf[ihex_len-2] , ihex_buf[ihex_len-1]) - (check & 0xff);
}

void pack_ihex_data(uint8_t *data_ptr, uint8_t data_len)
{
uint32_t 	i,data_idx;

	for(i=0,data_idx=0;i<data_len*2;i+=2,data_idx++)
	{
		IHex.ihex_line[data_idx] = ihex_byte(data_ptr[i],data_ptr[i+1]);
	}
	IHex.stored_bytes += data_len;
}

uint8_t	ihex_decode(uint8_t *data_ptr, uint8_t data_len)
{
uint8_t decoded_len = 255;
	if ( ihex_check_line(data_ptr,data_len))
		return decoded_len;
	IHex.record_type = ((data_ptr[7] & 0x0f)<<4 ) | (data_ptr[8] & 0x0f);
	switch(IHex.record_type)
	{
	case	IHEX_DATA	:
		IHex.address = (ihex_byte(data_ptr[3],data_ptr[4]) << 8) | ihex_byte(data_ptr[5],data_ptr[6]);
		decoded_len = ihex_byte(data_ptr[1],data_ptr[2]);
		pack_ihex_data(&data_ptr[9],decoded_len);
		break;
	case	IHEX_END_OF_FILE	:
		decoded_len = 0; // EOF
		break;
	case	IHEX_EXTENDED_SEGMENT_ADDRESS	:
		IHex.extended_address =
				(ihex_byte(data_ptr[3],data_ptr[4]) << 24) |
				(ihex_byte(data_ptr[5],data_ptr[6]) << 16) |
				(ihex_byte(data_ptr[7],data_ptr[8]) << 8 ) |
				(ihex_byte(data_ptr[9],data_ptr[10]));
		decoded_len = 0x80+IHEX_EXTENDED_SEGMENT_ADDRESS;
		break;
	case	IHEX_START_SEGMENT_ADDRESS	:
		IHex.start_segment_address =
				(ihex_byte(data_ptr[3],data_ptr[4]) << 24) |
				(ihex_byte(data_ptr[5],data_ptr[6]) << 16) |
				(ihex_byte(data_ptr[7],data_ptr[8]) << 8 ) |
				(ihex_byte(data_ptr[9],data_ptr[10]));
		decoded_len = 0x80+IHEX_START_SEGMENT_ADDRESS;
		break;
	case	IHEX_EXTENDED_LINEAR_ADDRESS	:
		IHex.extended_linear_address =
				(ihex_byte(data_ptr[9],data_ptr[10]) << 8 ) |
				(ihex_byte(data_ptr[11],data_ptr[12]));
		decoded_len = ihex_byte(data_ptr[1],data_ptr[2]);
		decoded_len = 0x80+IHEX_EXTENDED_LINEAR_ADDRESS;
		break;
	case	IHEX_START_LINEAR_ADDRESS	:
		IHex.start_linear_address =
				(ihex_byte(data_ptr[3],data_ptr[4]) << 24) |
				(ihex_byte(data_ptr[5],data_ptr[6]) << 16) |
				(ihex_byte(data_ptr[7],data_ptr[8]) << 8 ) |
				(ihex_byte(data_ptr[9],data_ptr[10]));
		decoded_len = 0x80+IHEX_START_LINEAR_ADDRESS;
		break;
	}
	return decoded_len;
}

uint32_t ihex_get_decoded_datalen(void)
{
	return IHex.stored_bytes;
}

uint8_t	ihex_init(uint8_t *dest_data_ptr)
{
	IHex.dest_data_ptr = dest_data_ptr;
	bzero(dest_data_ptr,131072);
	IHex.stored_bytes = 0;
	return 0;
}

#endif // #ifdef INTEL_HEX_DECODER_ENABLE
