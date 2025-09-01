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
 * sample_process_1_servo.c
 *
 *  Created on: Jan 23, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_SERVO

#define	SERVO_TIMER			htim16
#define	SERVO_CHANNEL		TIM_CHANNEL_1
#define	SERVO_PULSES		8
extern	TIM_HandleTypeDef 	SERVO_TIMER;

SERVO_Control_Drv_TypeDef	servo_ch1 =
{
	.servo_timer = &SERVO_TIMER,
	.servo_channel = SERVO_CHANNEL,
	.cycle_time = 20000,
	.repetition = 8,
	.min_time = 500,
	.max_time = 2500,
};
uint32_t		servo_ch1_driver_handle;

void sample_process_1_servo(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		state=0;

	create_timer(TIMER_ID_0,500,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	servo_ch1_driver_handle = servo_register(&servo_ch1);
	servo_start(servo_ch1_driver_handle);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			servo_set_position(servo_ch1_driver_handle,state*10,SERVO_PULSES);
			state++;
			if ( state == 11 )
				state = 0;

		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_BASIC
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
