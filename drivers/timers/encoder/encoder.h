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
 * encoder.h
 *
 *  Created on: Oct 6, 2025
 *      Author: fil
 */

#ifndef DRIVERS_TIMERS_ENCODER_ENCODER_H_
#define DRIVERS_TIMERS_ENCODER_ENCODER_H_

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
	uint32_t			wakeup_id;
	GPIO_TypeDef	 	*button_port;
	uint16_t			button_bit;
	uint32_t 			encoder_last_value;
	uint32_t 			encoder_value;
	void				(*irq_encoder_callback)  (uint32_t encoder_value);
}Encoder_Drv_TypeDef;
/* status */
#define	ENCODER_INITIALIZED			0x01
#define	ENCODER_WAKEUP				0x02
#define	ENCODER_UP					0x10
#define	ENCODER_DOWN				0x20
#define	ENCODER_RUNNING				0x40
#define	ENCODER_READY				0x80

extern uint32_t	encoder_register(Encoder_Drv_TypeDef *private_data);

#endif /* DRIVERS_TIMERS_ENCODER_ENCODER_H_ */
