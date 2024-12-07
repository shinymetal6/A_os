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
 * sample_process_1_dccpwm.c
 *
 *  Created on: Dec 4, 2024
 *      Author: fil
 */
#include "main.h"
#include "A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef DCC_ENABLE_GPIO_Port

extern	TIM_HandleTypeDef htim1;
DCC_Control_Drv_TypeDef	DCC_Control =
{
	.dcc_timer = &htim1,
	.timer_dcc_channel = TIM_CHANNEL_3,
	.timer_cutout_channel = TIM_CHANNEL_4,
	.enable_port = DCC_ENABLE_GPIO_Port,
	.enable_bit = DCC_ENABLE_Pin,
};
uint32_t		dcc_driver_handle;

extern	TIM_HandleTypeDef htim4;

Pwm_Control_TypeDef	Pwm_Control =
{
		.pwm_timer = &htim4,
		.pwm_channel = TIM_CHANNEL_4,
		.pulse_width[4] = 1000,
};

uint32_t		pwm_driver_handle;

void sample_process_1_dccpwm(uint32_t process_id)
{
uint32_t	wakeup,flags;

uint32_t	pw=0 , dir = 0;

	dcc_driver_handle = dcc_register(&DCC_Control,0,0);
	dcc_init(dcc_driver_handle);
	dcc_start(dcc_driver_handle);

	pwm_driver_handle = pwm_register(&Pwm_Control,0);
	pwm_init(pwm_driver_handle);
	pwm_start(pwm_driver_handle);

	create_timer(TIMER_ID_0,100,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	while(1)
	{
		wait_event(EVENT_TIMER);
		get_wakeup_flags(&wakeup,&flags);

		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			if ( dir != 0 )
			{
				pw += 1000;
				if (pw >= 10000)
					dir = 0;
				pwm_set_width(pwm_driver_handle,pw);
			}
			else
			{
				if ( pw )
					pw -= 1000;
				if (pw <= 1000)
					dir = 1;
				pwm_set_width(pwm_driver_handle,pw);
			}
		}
	}
}
#else
void sample_process_1_dccpwm(uint32_t process_id)
{
	wait_event(HW_SLEEP_FOREVER);
}
#endif // #ifdef DCC_ENABLE_GPIO_Port

#endif // #ifdef SAMPLE_PROCESSES_ENABLED




