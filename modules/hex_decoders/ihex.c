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
uint8_t		ihex_line[16];
uint8_t		record_type;
uint8_t		data_len;
uint32_t	address;
uint32_t	extended_address;
uint32_t	extended_linear_address;
uint32_t	start_segment_address;
uint32_t	start_linear_address;

void pack_ihex_data(uint8_t *data_ptr, uint8_t data_len)
{
uint32_t i,data_idx;
	for(i=0,data_idx=0;i<data_len;i+=2,data_idx++)
	{
		if (data_ptr[i] <= '9')
			ihex_line[data_idx] = (data_ptr[i] - '0') << 4;
		else
			ihex_line[data_idx] = ((data_ptr[i] - 'A') + 0x0a) << 4;
		if (data_ptr[i+1] <= '9')
			ihex_line[data_idx] |= (data_ptr[i+1] - '0');
		else
			ihex_line[data_idx] |= (data_ptr[i+1] - 'A') + 0x0a;
	}
}

uint8_t	*get_ihex_data(void)
{
	return ihex_line;
}

uint8_t	ihex_decode(uint8_t *data_ptr, uint8_t data_len)
{
uint8_t decoded_len = 0;

	record_type = ((data_ptr[7] & 0x0f)<<4 ) | (data_ptr[8] & 0x0f);
	switch(record_type)
	{
	case	IHEX_DATA	:
		address = (
				((data_ptr[3] & 0x0f)<<12 ) |
				((data_ptr[4] & 0x0f)<<8 )  |
				((data_ptr[5] & 0x0f)<<4 )  |
				(data_ptr[6] & 0x0f) );
		decoded_len = ((data_ptr[1] & 0x0f)<<4 ) | (data_ptr[2] & 0x0f);
		pack_ihex_data(&data_ptr[9],decoded_len);
		break;
	case	IHEX_END_OF_FILE	:
		break;
	case	IHEX_EXTENDED_SEGMENT_ADDRESS	:
		extended_address = (
				((data_ptr[3] & 0x0f)<<28 ) |
				((data_ptr[4] & 0x0f)<<24 ) |
				((data_ptr[5] & 0x0f)<<20 ) |
				((data_ptr[6] & 0x0f)<<16 ) |
				((data_ptr[7] & 0x0f)<<12 ) |
				((data_ptr[8] & 0x0f)<<8 ) |
				((data_ptr[9] & 0x0f)<<4 ) |
				(data_ptr[10] & 0x0f)
				);
		break;
	case	IHEX_START_SEGMENT_ADDRESS	:
		start_segment_address = (
				((data_ptr[3] & 0x0f)<<28 ) |
				((data_ptr[4] & 0x0f)<<24 ) |
				((data_ptr[5] & 0x0f)<<20 ) |
				((data_ptr[6] & 0x0f)<<16 ) |
				((data_ptr[7] & 0x0f)<<12 ) |
				((data_ptr[8] & 0x0f)<<8 ) |
				((data_ptr[9] & 0x0f)<<4 ) |
				(data_ptr[10] & 0x0f)
				);
		break;
	case	IHEX_EXTENDED_LINEAR_ADDRESS	:
		extended_linear_address = (
				((data_ptr[3] & 0x0f)<<28 ) |
				((data_ptr[4] & 0x0f)<<24 ) |
				((data_ptr[5] & 0x0f)<<20 ) |
				((data_ptr[6] & 0x0f)<<16 ) |
				((data_ptr[7] & 0x0f)<<12 ) |
				((data_ptr[8] & 0x0f)<<8 ) |
				((data_ptr[9] & 0x0f)<<4 ) |
				(data_ptr[10] & 0x0f)
				);
		break;
	case	IHEX_START_LINEAR_ADDRESS	:
		start_linear_address = (
				((data_ptr[3] & 0x0f)<<28 ) |
				((data_ptr[4] & 0x0f)<<24 ) |
				((data_ptr[5] & 0x0f)<<20 ) |
				((data_ptr[6] & 0x0f)<<16 ) |
				((data_ptr[7] & 0x0f)<<12 ) |
				((data_ptr[8] & 0x0f)<<8 ) |
				((data_ptr[9] & 0x0f)<<4 ) |
				(data_ptr[10] & 0x0f)
				);
		break;
	}
	return decoded_len;
}


#endif // #ifdef INTEL_HEX_DECODER_ENABLE
