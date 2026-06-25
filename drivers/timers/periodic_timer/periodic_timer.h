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
 * periodic_timer.h
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#ifndef DRIVERS_TIMERS_PERIODIC_TIMER_PERIODIC_TIMER_H_
#define DRIVERS_TIMERS_PERIODIC_TIMER_PERIODIC_TIMER_H_

typedef struct
{
	/* timer header */
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_timer;
	TIM_HandleTypeDef 	*timer;
	uint8_t				timer_type;
	/* timer internals */
	uint32_t			timer_channel;
	void				(*User_Callback)(uint32_t param);
}PERIODIC_Timer_DriverStruct_t;
/* status */
#define	PERIODIC_TIMER_INITIALIZED		0x01
#define	PERIODIC_TIMER_RUNNING			0x80

extern uint32_t	periodic_timer_register(PERIODIC_Timer_DriverStruct_t *periodic_timer_drv);
extern uint32_t periodic_timer_start(PERIODIC_Timer_DriverStruct_t *periodic_timer_drv);

#endif /* DRIVERS_TIMERS_PERIODIC_TIMER_PERIODIC_TIMER_H_ */
