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
 * timer.h
 *
 *  Created on: Nov 11, 2024
 *      Author: fil
 */

#ifndef KERNEL_TIMER_H_
#define KERNEL_TIMER_H_

extern	void 		DWT_Delay_us(uint32_t au32_microseconds);
extern	void 		task_delay(uint32_t tick_count);
extern	uint32_t 	create_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags);
extern	uint32_t 	start_timer(uint8_t timer_id);
extern	uint32_t 	restart_timer(uint8_t timer_id,uint32_t tick_count,uint8_t flags);
extern	uint32_t 	stop_timer(uint8_t timer_id);
extern	uint32_t 	destroy_timer(uint8_t timer_id);
extern	uint8_t 	get_timer_expired(void);
extern	int32_t 	A_GetTick(void);
extern	uint32_t 	set_before_check_timers_callback(void (*callback)(void));
extern	uint32_t 	unset_before_check_timers_callback(void (*callback)(void));
extern	uint32_t 	set_after_check_timers_callback(void (*callback)(void));
extern	uint32_t 	unset_after_check_timers_callback(void (*callback)(void));

#endif /* KERNEL_TIMER_H_ */
