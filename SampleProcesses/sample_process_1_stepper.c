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

/* Stepper */
void stepper_callback(uint32_t value);

Stepper_Control_DriverStruct_t	Stepper_Control =
{
		.timer = &htim16,
		.timer_channel = TIM_CHANNEL_1,
		.tim_port = PERI_STEP_TIM16CH1_GPIO_Port,
		.tim_bit = PERI_STEP_TIM16CH1_Pin,
		.dir_port = PERI_DIR_GPIO_Port,
		.dir_bit = PERI_DIR_Pin,
		.enable_port = SLEEP_3G_GPIO_Port,
		.enable_bit = SLEEP_3G_Pin,
		.pulse_width = STEPPER_DEFAULT_PW,
		.prescaler = 480,
		.steps_per_rotation = 200,
		.stepper_callback = stepper_callback,
};

uint8_t	stepper_running = 0;
uint32_t prescaler;
void stepper_callback(uint32_t value)
{
	stepper_set_prescaler(&Stepper_Control,prescaler);
	prescaler-=40;
	if ( prescaler < 360 )
		prescaler = 960;
	stepper_running = 0;
}

void sample_process_1_init(uint32_t process_id)
{
	stepper_register(&Stepper_Control);
	stepper_init(&Stepper_Control);
}

void sample_process_1_stepper(uint32_t process_id)
{
uint32_t	wakeup,flags;
	prescaler = 960;
	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			process_led();
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
			if (stepper_running == 0 )
			{
				stepper_running = 1;
				stepper_start(&Stepper_Control,TIM_CHANNEL_1,2,STEPPER_DIRECTION_FORWARD); // do 2 rotation @Stepper_Control.steps_per_rotation 400 pulses
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
			}
		}
	}
}


#endif
#endif //#ifdef SAMPLEPROCESS_1_STEPPER
