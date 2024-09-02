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
 * s3_hex.h
 *
 *  Created on: Sep 2, 2024
 *      Author: fil
 */


#ifndef MODULES_HEX_DECODERS_S3_HEX_H_
#define MODULES_HEX_DECODERS_S3_HEX_H_

typedef struct {
	uint8_t		s3hex_line[32];
	uint8_t		record_type;
	uint8_t		data_len;
	uint32_t	address;
	uint32_t	extended_address;
	uint32_t	extended_linear_address;
	uint32_t	start_segment_address;
	uint32_t	start_linear_address;
	uint32_t	absolute_address;
	uint32_t	stored_bytes;
} S3Hex_struct_t;


extern	uint8_t	*get_s3hex_data_ptr(void);
extern	uint32_t get_s3hex_address(void);

#endif /* MODULES_HEX_DECODERS_S3_HEX_H_ */
