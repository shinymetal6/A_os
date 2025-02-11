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
 * system_functions.h
 *
 *  Created on: Sep 11, 2024
 *      Author: fil
 */


#ifndef KERNEL_SYSTEM_FUNCTIONS_H_
#define KERNEL_SYSTEM_FUNCTIONS_H_

extern	void A_clear32(uint8_t	*ptr,uint32_t size);
extern	void A_copy32(uint8_t *src,uint8_t *dest, uint32_t size_in_bytes);
extern	uint8_t A_hex_to_byte(uint8_t byte1 , uint8_t byte2);

#endif /* KERNEL_SYSTEM_FUNCTIONS_H_ */
