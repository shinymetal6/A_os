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
#define	WS2812_SYNCLEN		500

#define	WS2812_MAX_BUFLEN	((WS2812_SYNCLEN*WS2812_LEDBPP)+(WS2812_MAX_NUMLEDS*WS2812_LEDBPP))

/* for timfreq = 170MHz */
#define WS2812_0	68
#define WS2812_1	136
#define WS2812_ARR	211
/* for timfreq = 80MHz */
/*
#define WS2812_0	32
#define WS2812_1	64
#define WS2812_ARR	99
*/
/* for timfreq = 480MHz */
/*
#define WS2812_0	192
#define WS2812_1	384
#define WS2812_ARR	596
*/

// Reset Head: 250 bits * 1.25µs = 312.5µs (Safely > 280µs for modern LEDs)
#define WS2812_RESET_HEAD 250
// Reset Tail: 250 bits * 1.25µs = 312.5µs (Safely > 280µs for modern LEDs)
#define WS2812_RESET_TAIL 250

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
	uint8_t 			ws2812_type;
	uint32_t 			ws2812_numleds;
	uint16_t 			*ws2812_work_buf;
	uint32_t 			ws2812_work_buf_buflen;
	uint16_t 			ws2812_one_val;
	uint16_t 			ws2812_zero_val;
	uint16_t 			ws2812_arr_val;
	void				(*irq_ws2812_callback)  (uint32_t parameter);
	uint32_t			wakeup_id;
}WS2812_DriverStruct_t;

extern uint32_t	ws2812_register(WS2812_DriverStruct_t *private_data);
extern void 	ws2812_ClearPixels(WS2812_DriverStruct_t *ws2812_drv);
extern void 	ws2812_SetPixel(WS2812_DriverStruct_t *ws2812_drv,uint16_t index, uint8_t r, uint8_t g, uint8_t b);
extern uint32_t ws2812_update(WS2812_DriverStruct_t *ws2812_drv);

#endif /* DRIVERS_TIMERS_WS2812_WS2812_H_ */
