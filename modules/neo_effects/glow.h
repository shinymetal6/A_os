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
 * glow.h
 *
 *  Created on: Jul 1, 2026
 *      Author: fil
 */

#ifndef MODULES_NEO_EFFECTS_GLOW_H_
#define MODULES_NEO_EFFECTS_GLOW_H_

typedef struct
{
	/* timer header */
	uint32_t 			*led_buf;
	uint8_t 			r;
	uint8_t 			g;
	uint8_t 			b;
	uint32_t 			glow_len;
	uint32_t 			glow_step;
	uint8_t 			direction;
	uint8_t 			initial_brightness;
	uint8_t 			current_brightness;
	void				(*glow_callback)  (uint32_t param);
}Glow_Struct_t;
/* direction */
#define	WS2812_GLOW_UP		1
#define	WS2812_GLOW_DOWN	0

extern uint32_t	glow_init(WS2812_DriverStruct_t *ws2812_drv,Glow_Struct_t *Glow);
extern uint32_t	glow_apply(WS2812_DriverStruct_t *ws2812_drv,Glow_Struct_t *Glow);


#endif /* MODULES_NEO_EFFECTS_GLOW_H_ */
