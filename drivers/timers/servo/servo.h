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
 * servo.h
 *
 *  Created on: Jan 23, 2025
 *      Author: fil
 */
#ifndef DRIVERS_TIMERS_SERVO_SERVO_H_
#define DRIVERS_TIMERS_SERVO_SERVO_H_

typedef struct
{
	uint8_t					status;
	uint8_t					flags;
	uint8_t					handle;
	uint8_t					repetition;
	uint32_t				cycle_time;
	TIM_HandleTypeDef 		*servo_timer;
	uint32_t 				servo_channel;

}SERVO_Control_Drv_TypeDef;

extern uint32_t servo_start(uint8_t handle);
extern uint32_t servo_stop(uint8_t handle);
extern uint32_t servo_get_status(uint8_t handle);
extern uint32_t servo_set_prescaler(uint8_t handle,uint32_t prescaler);
extern uint32_t servo_set_position(uint8_t handle,uint8_t servo_position,uint8_t servo_pulses);
extern uint32_t	servo_register(SERVO_Control_Drv_TypeDef *private_data);

#endif /* DRIVERS_TIMERS_SERVO_SERVO_H_ */
