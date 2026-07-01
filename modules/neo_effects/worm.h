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
 * worm.h
 *
 *  Created on: Jun 30, 2026
 *      Author: fil
 */

#ifndef MODULES_NEO_EFFECTS_WORM_H_
#define MODULES_NEO_EFFECTS_WORM_H_

typedef struct
{
	/* timer header */
	uint32_t 			position;
	uint32_t 			worm_head;
	uint32_t 			worm_len;
	uint8_t 			direction;
	uint32_t 			*led_buf;
	uint8_t 			r;
	uint8_t 			g;
	uint8_t 			b;
	void				(*worm_callback)  (uint32_t param);
}Worm_Struct_t;


extern	uint32_t	worm_init(WS2812_DriverStruct_t *ws2812_drv,Worm_Struct_t *Worm);
extern	uint32_t	worm_advance(WS2812_DriverStruct_t *ws2812_drv,Worm_Struct_t *Worm);


#endif /* MODULES_NEO_EFFECTS_WORM_H_ */
