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
 * ws2812.h
 *
 *  Created on: Feb 9, 2026
 *      Author: fil
 */

#ifndef DRIVERS_TIMERS_WS2812_WS2812_H_
#define DRIVERS_TIMERS_WS2812_WS2812_H_

#define	WS2812_LEDBPP		24
#define	WS2812_MAX_NUMLEDS	10

// Some WS2812 variants (especially clones or long strips) require ≥256 µs reset pulse instead of the standard 50 µs spec
#define	WS2812_SYNCLEN		192

#define	WS2812_MAX_BUFLEN	((WS2812_SYNCLEN*WS2812_LEDBPP)+(WS2812_MAX_NUMLEDS*WS2812_LEDBPP))

#define	GREEN_SHIFT		0
#define	RED_SHIFT		8
#define	BLUE_SHIFT		16

typedef struct
{
	/* timer header */
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_timer;
	TIM_HandleTypeDef 	*ws2812_timer;
	uint8_t				timer_type;
	/* timer internals */
	uint8_t				handle;
	uint32_t 			ws2812_timer_channel;
	uint32_t 			ws2812_numleds;
	uint32_t 			*ws2812_work_buf;
	uint32_t 			ws2812_work_buf_buflen;
	uint32_t			wakeup_id;
}WS2812_Drv_TypeDef;

extern uint32_t	ws2812_register(WS2812_Drv_TypeDef *private_data);
extern void 	ws2812_ClearPixels(WS2812_Drv_TypeDef *ws2812_drv);
extern void 	ws2812_SetPixel(WS2812_Drv_TypeDef *ws2812_drv,uint32_t location, uint8_t r,uint8_t g,uint8_t b);


#endif /* DRIVERS_TIMERS_WS2812_WS2812_H_ */
