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
 * ihex.h
 *
 *  Created on: Aug 8, 2024
 *      Author: fil
 */

#ifndef MODULES_HEX_DECODERS_IHEX_H_
#define MODULES_HEX_DECODERS_IHEX_H_

#define	IHEX_DATA						0x00
#define	IHEX_END_OF_FILE				0x01
#define	IHEX_EXTENDED_SEGMENT_ADDRESS	0x02
#define	IHEX_START_SEGMENT_ADDRESS		0x03
#define	IHEX_EXTENDED_LINEAR_ADDRESS	0x04
#define	IHEX_START_LINEAR_ADDRESS		0x05

typedef struct {
	uint8_t		ihex_line[32];
	uint8_t		record_type;
	uint8_t		data_len;
	uint32_t	address;
	uint32_t	extended_address;
	uint32_t	extended_linear_address;
	uint32_t	start_segment_address;
	uint32_t	start_linear_address;
	uint32_t	absolute_address;
	uint8_t		*dest_data_ptr;
	uint8_t		*dest_data_current_ptr;
	uint32_t	stored_bytes;
} IHex_struct_t;

extern	uint8_t	ihex_init(uint8_t *dest_data_ptr);
extern	uint8_t	ihex_decode(uint8_t *data_ptr, uint8_t data_len);
extern	uint32_t ihex_get_decoded_datalen(void);
extern	uint32_t ihex_check_line(uint8_t *ihex_buf , uint32_t ihex_len);
extern	uint32_t ihex_get_crlflen(uint8_t *data_ptr);
extern	uint32_t ihex_get_data_len(uint8_t *data_ptr);
extern	uint8_t	*get_ihex_data_ptr(void);
extern	uint32_t get_ihex_address(void);

#endif /* MODULES_HEX_DECODERS_IHEX_H_ */
