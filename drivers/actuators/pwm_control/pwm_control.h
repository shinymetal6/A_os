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
	uint32_t			prescaler;
	uint32_t			pulse_width;
}Pwm_Control_Drv_TypeDef;
/* status */
#define	PWM_CHANNEL_INITIALIZED		0x01
#define	PWM_CHANNEL_RUNNING			0x80

#define	PWM_EA_SET_PWM_PRESCALER	1
#define	PWM_EA_SET_PWM_PERIOD		2
#define	PWM_EA_SET_PWM_DIRECTION	3

extern	uint32_t pwm_control_get_drv_struct(DriversDefs_t *new_struct,uint8_t peripheral_index);

#endif /* DRIVERS_ACTUATORS_PWM_CONTROL_PWM_CONTROL_H_ */
