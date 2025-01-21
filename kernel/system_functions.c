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
//#include "kernel_opt.h"
#include "system_functions.h"

uint32_t A_bit_index_to_num(uint32_t bit_index )
{
uint8_t shft = 0;
	while(((bit_index >> shft) & 1) == 0)
		shft++;
	return shft;
}

uint32_t time_start;
uint32_t usec_elapsed;
void A_get_timelapse_start(void)
{
	time_start = DWT->CYCCNT;
}

uint32_t A_get_timelapse_end(void)
{
    usec_elapsed = (DWT->CYCCNT - time_start)/ (SYSTICK_TIM_CLK/1000000) ;
    return	usec_elapsed;
}

void A_clear32(uint8_t	*ptr,uint32_t size_in_bytes)
{
uint32_t	i;
uint32_t *ptr2 = (uint32_t *)ptr;
	__disable_irq();
	for(i=0;i<size_in_bytes/sizeof(uint32_t);i++)
		*ptr2++ = 0;
	__enable_irq();
}

void A_copy32(uint8_t *src,uint8_t *dest, uint32_t size_in_bytes)
{
uint32_t	i;
uint32_t *src32 = (uint32_t *)src, *dest32 = (uint32_t *)dest;

	__disable_irq();
	for(i=0;i<size_in_bytes/sizeof(uint32_t);i++)
		*dest32++ = *src32++;
	__enable_irq();
}

