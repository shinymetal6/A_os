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
 * timers.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_TIMERS_TIMERS_H_
#define DRIVERS_TIMERS_TIMERS_H_

#ifdef A_OS_TIMERS_ENABLED

/* timer_type */
#define TIM_TYPE_ENCODER		1
#define TIM_TYPE_DHT11			2
#define TIM_TYPE_PERIODIC		3

typedef struct
{
	/* timer header */
	uint8_t 				process;
	uint8_t					status;
	uint8_t					flags;
	uint32_t 				*next_timer;
	TIM_HandleTypeDef 		*timer;
	uint8_t					timer_type;
}TIMER_DriverStruct_t;

#include "dcc/dcc.h"
#include "pwm/pwm.h"
#include "servo/servo.h"
#include "ws2812/ws2812.h"
#include "encoder/encoder.h"
#include "dhtxx_am230x/dhtxx_am230x.h"
#include "periodic_timer/periodic_timer.h"

extern	TIMER_DriverStruct_t	*timer_drv_ptr;

#endif // #ifdef A_OS_TIMERS_ENABLED

#endif /* DRIVERS_TIMERS_TIMERS_H_ */
