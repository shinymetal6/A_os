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
 * dmx512.h
 *
 *  Created on: Mar 23, 2024
 *      Author: fil
 */

#ifndef MODULES_DMX512_DMX512_H_
#define MODULES_DMX512_DMX512_H_

#define	DMX_LEN	513
typedef struct
{
	uint32_t	uart;
	uint8_t		DMXbuf[DMX_LEN];
	uint8_t		state;
	uint8_t		flag;
}dmx_t;

extern	void dmx512_init(uint32_t uart);
extern	void dmx512_start(void);


#endif /* MODULES_DMX512_DMX512_H_ */
