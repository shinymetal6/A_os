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
 * sample_process_1_stepper.c
 *
 *  Created on: Jun 9, 2026
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_STEPPER

#define	STEPPER_TIMER		htim16
#define	STEPPER_CHANNEL		TIM_CHANNEL_1
#define	STEPPER_PULSES		8
extern	TIM_HandleTypeDef 	STEPPER_TIMER;
#define	STEPPER_DEFAULT_PW	500
#define	STEPPER_FORWARD		1
#define	STEPPER_REVERSE		0

/* Stepper */
void stepper_callback(uint32_t value);

Stepper_Control_DriverStruct_t	Stepper_Control =
{
		.timer = &htim16,
		.tim_port = STEP_TIM_16_CH1_GPIO_Port,
		.tim_bit = STEP_TIM_16_CH1_Pin,
		.dir_port = STEP_DIR_GPIO_Port,
		.dir_bit = STEP_DIR_Pin,
		.pulse_width = STEPPER_DEFAULT_PW,
		.steps_per_rotation = 4,
		.stepper_callback = stepper_callback,
};

void stepper_callback(uint32_t value)
{
	stepper_stop(&Stepper_Control,TIM_CHANNEL_1);
}

void sample_process_1_init(uint32_t process_id)
{
	stepper_register(&Stepper_Control);
	stepper_init(&Stepper_Control);
}

void sample_process_1_stepper(uint32_t process_id)
{
uint32_t	wakeup,flags;
	create_timer(TIMER_ID_0,1000,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			stepper_start(&Stepper_Control,TIM_CHANNEL_1,2,STEPPER_FORWARD); // do 2 rotation @Stepper_Control.steps_per_rotation 400 pulses

		}
	}
}


#endif
#endif //#ifdef SAMPLEPROCESS_1_STEPPER
