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
 * stepper.h
 *
 *  Created on: Jun 9, 2026
 *      Author: fil
 */

#ifndef DRIVERS_TIMERS_STEPPER_STEPPER_H_
#define DRIVERS_TIMERS_STEPPER_STEPPER_H_

typedef struct
{
	/* timer header */
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_timer;
	TIM_HandleTypeDef 	*timer;
	uint8_t				timer_type;
	uint32_t			wakeup_id;
	/* timer internals */
	uint32_t 			timer_channel;
	GPIO_TypeDef	 	*tim_port;
	uint16_t			tim_bit;
	GPIO_TypeDef	 	*dir_port;
	uint16_t			dir_bit;
	uint8_t				direction;
	GPIO_TypeDef	 	*enable_port;
	uint16_t			enable_bit;
	uint32_t 			prescaler;
	uint32_t 			period;
	uint32_t 			pulse_width;
	uint32_t			number_of_steps;
	uint32_t			stored_number_of_steps;
	uint32_t			steps_correction_factor;
	uint32_t			steps_per_rotation;
	uint32_t			number_of_rotation;
	void				(*stepper_callback)  (uint32_t param);
}Stepper_Control_DriverStruct_t;
#define	STEPPER_CHANNEL_ENABLED		0x01
#define	STEPPER_CHANNEL_STARTED		0x20
#define	STEPPER_CHANNEL_READY		0x40
#define	STEPPER_CHANNEL_INITIALIZED	0x80

#define	STEPPER_DEFAULT_STEP_PER_ROTATION	199
#define	STEPPER_DIRECTION_FORWARD			0x01
#define	STEPPER_DIRECTION_REVERSE			0x00
#define	STEPPER_DEFAULT_PRESCALER			480
#define	STEPPER_DEFAULT_PULSE_WIDTH			500
#define	STEPPER_DEFAULT_PERIOD				1000

#define	STEPPER_RCR_MAXVAL					199

extern	uint32_t	stepper_register(Stepper_Control_DriverStruct_t *stepper_drv);
extern	uint32_t 	stepper_init(Stepper_Control_DriverStruct_t *stepper_drv);
extern	uint32_t	stepper_start(Stepper_Control_DriverStruct_t *stepper_drv,uint32_t stepper_channel,uint8_t	number_of_rotation,uint8_t direction);
extern	uint32_t	stepper_stop(Stepper_Control_DriverStruct_t *stepper_drv,uint32_t stepper_channel);
extern	uint32_t	stepper_set_prescaler(Stepper_Control_DriverStruct_t *stepper_drv,uint32_t prescaler);

extern	void		stepper_internal_callback(Stepper_Control_DriverStruct_t *stepper_drv);


#endif /* DRIVERS_TIMERS_STEPPER_STEPPER_H_ */
