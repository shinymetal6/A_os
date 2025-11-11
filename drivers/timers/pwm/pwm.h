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
	/* timer header */
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_timer;
	TIM_HandleTypeDef 	*timer;
	uint8_t				timer_type;
	/* timer internals */
	uint8_t				handle;
	uint32_t 			pwm_channel;
	GPIO_TypeDef	 	*enable_port;
	uint16_t			enable_bit;
	uint32_t 			prescaler;
	uint32_t 			period;
	uint32_t 			pulse_width[6];
	uint8_t 			pwm_direction;
}Pwm_Control_TypeDef;

/* status */
#define	PWM_CHANNEL_INITIALIZED		0x01
#define	PWM_CHANNEL_RUNNING			0x80

extern	uint32_t pwm_init(Pwm_Control_TypeDef *pwm_drv);
extern	uint32_t pwm_start(Pwm_Control_TypeDef *pwm_drv);
extern	uint32_t pwm_stop(Pwm_Control_TypeDef *pwm_drv);
extern	uint32_t pwm_get_status(Pwm_Control_TypeDef *pwm_drv);
extern	uint32_t pwm_set_prescaler(Pwm_Control_TypeDef *pwm_drv,uint32_t prescaler);
extern	uint32_t pwm_set_period(Pwm_Control_TypeDef *pwm_drv,uint32_t period);
extern	uint32_t pwm_set_width(Pwm_Control_TypeDef *pwm_drv,uint32_t pulse_width);
extern	uint32_t pwm_set_direction(Pwm_Control_TypeDef *pwm_drv,uint8_t pwm_direction);
extern	uint32_t pwm_register(Pwm_Control_TypeDef *tim_driver_private_data);

#endif /* DRIVERS_TIMERS_PWM_PWM_H_ */
