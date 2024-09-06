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
 * s3_hex.c
 *
 *  Created on: Sep 2, 2024
 *      Author: fil
 */

#include "main.h"
#include "../../kernel/system_default.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"
#include "../../kernel/scheduler.h"
//#include "../../kernel/kernel_opt.h"

#ifdef MOTOROLA_HEX_DECODER_ENABLE

#include "s3_hex.h"
#include <string.h>

SYSTEM_RAM	S3Hex_struct_t	S3Hex;

uint8_t	*get_s3hex_data_ptr(void)
{
	return S3Hex.s3hex_line;
}

uint32_t get_s3hex_address(void)
{
	return S3Hex.address;
}

uint32_t s3hex_check_line(uint8_t *data_ptr , uint32_t data_len)
{
uint32_t	check=0 , i;

	for(i=1;i<data_len;i+=2)
		check += to_hex_byte(data_ptr[i] , data_ptr[i+1]);
	return to_hex_byte(data_ptr[data_len-2] , data_ptr[data_len-1]) - (check & 0xff);
}

void pack_s3hex_data(uint8_t *data_ptr, uint8_t data_len)
{
uint32_t 	i,data_idx;

	for(i=0,data_idx=0;i<data_len*2;i+=2,data_idx++)
	{
		S3Hex.s3hex_line[data_idx] = to_hex_byte(data_ptr[i],data_ptr[i+1]);
	}
	S3Hex.stored_bytes += data_len;
}

uint8_t	s3hex_decode(uint8_t *data_ptr, uint8_t data_len)
{
uint8_t decoded_len = 255;

	if ( s3hex_check_line(data_ptr , data_len) == 0 )
	{
		S3Hex.record_type = data_ptr[1];
		switch(S3Hex.record_type)
		{
		case '0' :
			break;
		case '3' :
			decoded_len = S3Hex.data_len = to_hex_byte(data_ptr[2],data_ptr[3]);
			S3Hex.address =
					(to_hex_byte(data_ptr[4],data_ptr[5]) << 24) |
					(to_hex_byte(data_ptr[6],data_ptr[7]) << 16) |
					(to_hex_byte(data_ptr[8],data_ptr[9]) << 8 ) |
					(to_hex_byte(data_ptr[10],data_ptr[11]));
			pack_s3hex_data(&data_ptr[12],decoded_len);

			break;
		case '7' :
		case '8' :
		case '9' :
			decoded_len = 0; // EOF
			break;
		}
	}
	return decoded_len;
}

#endif // #ifdef MOTOROLA_HEX_DECODER_ENABLE
