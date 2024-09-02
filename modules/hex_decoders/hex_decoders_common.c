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
 * hex_decoders_common.c
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

uint8_t to_hex_byte(uint8_t byte1 , uint8_t byte2)
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

uint32_t get_hex_crlflen(uint8_t *data_ptr)
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

