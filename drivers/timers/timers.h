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

typedef struct
{
	uint8_t 	process;
	uint8_t		status;
	uint8_t		flags;
	uint8_t		handle;
	uint32_t	*private_data;
}TIM_DriverStruct_t;

#include "dcc/dcc.h"
#include "pwm/pwm.h"
#include "servo/servo.h"
#include "ws2812/ws2812.h"

#endif // #ifdef A_OS_TIMERS_ENABLED

#endif /* DRIVERS_TIMERS_TIMERS_H_ */
