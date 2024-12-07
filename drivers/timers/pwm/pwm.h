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
 * pwm.h
 *
 *  Created on: Nov 12, 2024
 *      Author: fil
 */

#ifndef DRIVERS_TIMERS_PWM_PWM_H_
#define DRIVERS_TIMERS_PWM_PWM_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	TIM_HandleTypeDef 	*pwm_timer;
	uint32_t 			pwm_channel;
	GPIO_TypeDef	 	*enable_port;
	uint16_t			enable_bit;
	uint32_t 			prescaler;
	uint32_t 			pulse_width[6];
	uint8_t 			pwm_direction;
}Pwm_Control_TypeDef;

/* status */
#define	PWM_CHANNEL_INITIALIZED		0x01
#define	PWM_CHANNEL_RUNNING			0x80

extern	uint32_t pwm_init(uint8_t handle);
extern	uint32_t pwm_start(uint8_t handle);
extern	uint32_t pwm_stop(uint8_t handle);
extern	uint32_t pwm_get_status(uint8_t handle);
extern	uint32_t pwm_set_prescaler(uint8_t handle,uint32_t prescaler);
extern	uint32_t pwm_set_width(uint8_t handle,uint32_t pulse_width);
extern	uint32_t pwm_set_direction(uint8_t handle,uint8_t pwm_direction);
extern	uint32_t pwm_register(Pwm_Control_TypeDef *tim_driver_private_data,uint32_t driver_flags);

#endif /* DRIVERS_TIMERS_PWM_PWM_H_ */
