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

extern	uint8_t	ihex_decode(uint8_t *data_ptr, uint8_t data_len);
extern	uint8_t	*get_ihex_data(void);



#endif /* MODULES_HEX_DECODERS_IHEX_H_ */
