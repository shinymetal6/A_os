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
 * hex_decoders_common.h
 *
 *  Created on: Sep 2, 2024
 *      Author: fil
 */


#ifndef MODULES_HEX_DECODERS_HEX_DECODERS_COMMON_H_
#define MODULES_HEX_DECODERS_HEX_DECODERS_COMMON_H_

extern	uint8_t to_hex_byte(uint8_t byte1 , uint8_t byte2);
extern	uint32_t get_hex_crlflen(uint8_t *data_ptr);


#endif /* MODULES_HEX_DECODERS_HEX_DECODERS_COMMON_H_ */
