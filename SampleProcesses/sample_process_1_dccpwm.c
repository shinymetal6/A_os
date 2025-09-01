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
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_DCCPWM

#define	CH1_DCC_TIMER			htim1
#define	CH1_DCC_TIMER_PWM		TIM_CHANNEL_1
#define	CH1_DCC_TIMER_CUTOUT	TIM_CHANNEL_2
extern	TIM_HandleTypeDef 		CH1_DCC_TIMER;

#define	CH2_DCC_TIMER			htim15
#define	CH2_DCC_TIMER_PWM		TIM_CHANNEL_1
#define	CH2_DCC_TIMER_CUTOUT	TIM_CHANNEL_2
extern	TIM_HandleTypeDef 		CH2_DCC_TIMER;

#define	BACKLIGHT_TIMER			htim3
#define	BACKLIGHT_TIMER_CHANNEL	TIM_CHANNEL_2
extern	TIM_HandleTypeDef 		BACKLIGHT_TIMER;

DCC_Control_Drv_TypeDef	DCC_Control_ch1 =
{
	.dcc_timer = &CH1_DCC_TIMER,
	.timer_dcc_channel = CH1_DCC_TIMER_PWM,
	.timer_cutout_channel = CH1_DCC_TIMER_CUTOUT,
	.enable_port = DCC_ENABLE_GPIO_Port,
	.enable_bit = DCC_ENABLE_Pin,
};
uint32_t		dcc_ch1_driver_handle;

DCC_Control_Drv_TypeDef	DCC_Control_ch2 =
{
	.dcc_timer = &CH2_DCC_TIMER,
	.timer_dcc_channel = CH2_DCC_TIMER_PWM,
	.timer_cutout_channel = CH2_DCC_TIMER_CUTOUT,
	.enable_port = DCC_ENABLE_GPIO_Port,
	.enable_bit = DCC_ENABLE_Pin,
};
uint32_t		dcc_ch2_driver_handle;

Pwm_Control_TypeDef	Pwm_Backlight_Control =
{
		.pwm_timer = &BACKLIGHT_TIMER,
		.pwm_channel = BACKLIGHT_TIMER_CHANNEL,
		.pulse_width[4] = 1000,
};

uint32_t		backlight_pwm_driver_handle;

void sample_process_1_dccpwm(uint32_t process_id)
{
uint32_t	wakeup,flags;

uint32_t	pw=0 , dir = 0;

	dcc_ch1_driver_handle = dcc_register(&DCC_Control_ch1);
	dcc_init(dcc_ch1_driver_handle);
	dcc_start(dcc_ch1_driver_handle);
	dcc_ch2_driver_handle = dcc_register(&DCC_Control_ch2);
	dcc_init(dcc_ch2_driver_handle);
	dcc_start(dcc_ch2_driver_handle);

	backlight_pwm_driver_handle = pwm_register(&Pwm_Backlight_Control);
	pwm_init(backlight_pwm_driver_handle);
	pwm_start(backlight_pwm_driver_handle);

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
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
				pwm_set_width(backlight_pwm_driver_handle,pw);
			}
			else
			{
				if ( pw )
					pw -= 1000;
				if (pw <= 1000)
					dir = 1;
				pwm_set_width(backlight_pwm_driver_handle,pw);
			}
			process_led();
		}
	}
}
#endif // #ifdef SAMPLEPROCESS_1_DCCPWM
#endif // #ifdef SAMPLE_PROCESSES_ENABLED




