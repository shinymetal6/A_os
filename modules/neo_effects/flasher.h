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
 * flasher.h
 *
 *  Created on: Jul 1, 2026
 *      Author: fil
 */

#ifndef MODULES_NEO_EFFECTS_FLASHER_H_
#define MODULES_NEO_EFFECTS_FLASHER_H_

typedef struct
{
	/* timer header */
	NeoPixel_Struct_t	*led_buf;
	uint8_t 			r_on;
	uint8_t 			g_on;
	uint8_t 			b_on;
	uint8_t 			r_off;
	uint8_t 			g_off;
	uint8_t 			b_off;
	uint8_t 			brightness;
	uint32_t 			flasher_len;
	void				(*flasher_callback)  (uint32_t param);
}Flasher_Struct_t;
extern	uint32_t	flasher_init(WS2812_DriverStruct_t *ws2812_drv,Flasher_Struct_t *Flasher);
extern	uint32_t	flasher_apply(WS2812_DriverStruct_t *ws2812_drv,Flasher_Struct_t *Flasher,uint8_t state);

#endif /* MODULES_NEO_EFFECTS_FLASHER_H_ */
