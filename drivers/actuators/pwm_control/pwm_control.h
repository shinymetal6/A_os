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
 * pwm_control.h
 *
 *  Created on: Oct 14, 2024
 *      Author: fil
 */

#ifndef DRIVERS_ACTUATORS_PWM_CONTROL_PWM_CONTROL_H_
#define DRIVERS_ACTUATORS_PWM_CONTROL_PWM_CONTROL_H_

typedef struct
{
	uint8_t				status;
	uint8_t				flags;
	uint8_t				handle;
	DMA_HandleTypeDef 	*hdma[2];
	TIM_HandleTypeDef 	*pwm_timer;
	uint16_t 			pwm_channel;
	GPIO_TypeDef	 	*enable_port[1];
	uint16_t			enable_bit[1];
}Pwm_Control_Drv_TypeDef;

/* status */
#define	PWM_CHANNEL_INITIALIZED		0x01
#define	PWM_CHANNEL_RUNNING			0x80

typedef struct
{
	uint8_t				action;
	uint32_t			prescaler;
	uint32_t			pulse_width;
	uint8_t				pwm_direction;
}Pwm_Control_Actions_TypeDef;
/* action */
#define	PWM_EA_SET_PWM_PRESCALER	1
#define	PWM_EA_SET_PWM_PERIOD		2
#define	PWM_EA_SET_PWM_DIRECTION	3
/* pwm_direction */
#define	PWM_EA_PWM_DIRECTION_FWD	1
#define	PWM_EA_PWM_DIRECTION_REV	0


extern	uint32_t pwm_control_allocate_driver(DriverStruct_t *new_struct);

#endif /* DRIVERS_ACTUATORS_PWM_CONTROL_PWM_CONTROL_H_ */
