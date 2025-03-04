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
 *  Created on: Feb 24, 2025
 *      Author: fil
 */

#ifndef DRIVERS_TIMERS_WS2812_WS2812_H_
#define DRIVERS_TIMERS_WS2812_WS2812_H_

#define	LEDBPP		24
#define	NUMLEDS		45

#define	SYNCLEN		50
#define	BUFLEN		((SYNCLEN*LEDBPP)+(NUMLEDS*LEDBPP))

#define	PATTERN_0		35
#define	PATTERN_1		80

#define	GREEN_SHIFT		16
#define	RED_SHIFT		8
#define	BLUE_SHIFT		0

typedef struct
{
	uint8_t					status;
	uint8_t					flags;
	uint8_t					handle;
	TIM_HandleTypeDef 		*ws2812_timer;
	uint32_t 				ws2812_channel;
}WS2812_Drv_TypeDef;

extern uint32_t	ws2812_register(Pwm_Control_TypeDef *private_data);


#endif /* DRIVERS_TIMERS_WS2812_WS2812_H_ */
