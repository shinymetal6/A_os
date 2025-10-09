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
 * system_functions.c
 *
 *  Created on: Sep 18, 2023
 *      Author: fil
 */


#include "main.h"
#include "A.h"
#include "A_exported_functions.h"

ITCM_AREA_CODE uint32_t A_bit_index_to_num(uint32_t bit_index )
{
uint8_t shft = 0;
	while(((bit_index >> shft) & 1) == 0)
		shft++;
	return shft;
}

SYSTEM_RAM	uint32_t time_start;
SYSTEM_RAM	uint32_t usec_elapsed;
ITCM_AREA_CODE void A_get_timelapse_start(void)
{
	time_start = DWT->CYCCNT;
}

ITCM_AREA_CODE uint32_t A_get_timelapse_end(void)
{
    usec_elapsed = (DWT->CYCCNT - time_start)/ (SYSTICK_TIM_CLK/1000000) ;
    return	usec_elapsed;
}

ITCM_AREA_CODE void A_clear32(uint8_t	*ptr,uint32_t size_in_bytes)
{
uint32_t	i;
uint32_t *ptr2 = (uint32_t *)ptr;
	__disable_irq();
	for(i=0;i<size_in_bytes/sizeof(uint32_t);i++)
		*ptr2++ = 0;
	__enable_irq();
}

ITCM_AREA_CODE void A_copy32(uint8_t *src,uint8_t *dest, uint32_t size_in_bytes)
{
uint32_t	i;
uint32_t *src32 = (uint32_t *)src, *dest32 = (uint32_t *)dest;

	__disable_irq();
	for(i=0;i<size_in_bytes/sizeof(uint32_t);i++)
		*dest32++ = *src32++;
	__enable_irq();
}

ITCM_AREA_CODE uint8_t A_hex_to_byte(uint8_t byte1 , uint8_t byte2)
{
uint8_t	packed_byte;
	if ( byte1 <= '9' )
		packed_byte = (byte1 - '0') << 4;
	else
	{
		if (( byte1 >= 'A' ) && ( byte1 <= 'Z' ))
			packed_byte = ((byte1 - 'A')+10) << 4;
		else
			packed_byte = ((byte1 - 'a')+10) << 4;
	}
	if ( byte2 <= '9' )
		packed_byte |= (byte2 - '0');
	else
	{
		if (( byte2 >= 'A' ) && ( byte2 <= 'Z' ))
			packed_byte |= ((byte2 - 'A')+10);
		else
			packed_byte |= ((byte2 - 'a')+10);
	}
	return packed_byte;
}

