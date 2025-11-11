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
 * sample_process_1_pid.c
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#include "main.h"
#include "sample_A_os_includes.h"
#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#ifdef SAMPLEPROCESS_1_PID
extern	ADC_HandleTypeDef hadc1;
extern	TIM_HandleTypeDef htim6;
extern	TIM_HandleTypeDef htim7;
extern	TIM_HandleTypeDef htim16;

#define	ADC1_CHANNELS	6
#define	ADC1_MAX_VALUE	65535
#define PWM_MAX_VALUE 9999.0f  // Max value set in PWM_TIMER ARR (adjust based on your PWM config)

__attribute__ ((aligned (32)))	uint16_t	adc1_buf[ADC1_CHANNELS];
__attribute__ ((aligned (32))) ADC_DriverStruct_t	ADC1_Drv =
{
	.adc = &hadc1,
	.adc_timer = &htim6,
	.adc_buffer = adc1_buf,
	.num_channels = ADC1_CHANNELS,
};

extern	void pid_process(void);
PERIODIC_Timer_Drv_TypeDef Periodic_Timer =
{
	.timer = &htim7,
	.timer_channel = 0,
};

Pwm_Control_TypeDef	Pwm_Control =
{
	.timer = &htim16,
	.pwm_channel = TIM_CHANNEL_1,
	.period = 10000,
	.pulse_width = {5000, },
};

PIDController_TypeDef	Pid =
{
	.Kp = 2.0f,   						// Proportional gain (Tune these!)
	.Ki = 0.5f,   						// Integral gain
	.Kd = 0.1f,   						// Derivative gain
	.output_min = 0.0f, 				// Minimum output value
	.output_max = 9999.0f, 				// Maximum output value
	.setpoint = (ADC1_MAX_VALUE/2.0F), 	// Maximum output value
	.dt = 0.01F,						// Time step in seconds (e.g., 10ms = 0.01s) - Match PID timer frequency
	.periodic_timer = &Periodic_Timer,
	.User_Callback = pid_process,
};
uint32_t	pid_time_start;
uint32_t	pid_time_loop;
uint8_t	called = 0;
// --- Timer Interrupt Callback (Called every dt seconds) ---
void pid_process(void)
{
	if ( called == 0 )
	{
		pid_time_start = DWT->CYCCNT;
		called = 1;
	}
	else
	{
		pid_time_loop = (DWT->CYCCNT - pid_time_start) / (HSI_CLOCK / 1000000);
		called = 0;
	}

	// 1. Read the Process Variable (PV)
	float current_pv = (float )adc1_buf[2];
	// 2. Compute PID Output
	float control_output = PID_Compute(&Pid, current_pv);
	// 3. Apply Control Output (u(t))
    uint32_t pwm_compare_value = (uint32_t)control_output;
    if (pwm_compare_value > (uint32_t)PWM_MAX_VALUE)
        pwm_compare_value = (uint32_t)PWM_MAX_VALUE;
    pwm_set_width(&Pwm_Control,pwm_compare_value);
}

void sample_process_1_init(uint32_t process_id)
{
	adc_register(&ADC1_Drv);
	adc_start(&ADC1_Drv);
	pwm_register(&Pwm_Control);
}

void sample_process_1_pid(uint32_t process_id)
{
uint32_t	wakeup,flags;
uint8_t		cntr = 0;

	create_timer(TIMER_ID_0,10,TIMERFLAGS_FOREVER | TIMERFLAGS_ENABLED);
	PID_register(&Pid);
	pwm_start(&Pwm_Control);
	//HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);

	while(1)
	{
		wait_event(EVENT_TIMER|EVENT_USB_DEVICE_IRQ|EVENT_ADC1_IRQ);
		get_wakeup_flags(&wakeup,&flags);
		if (( wakeup & WAKEUP_FROM_TIMER) == WAKEUP_FROM_TIMER)
		{
			cntr++;
			if ( cntr == 10)
			{
				cntr = 0;
				process_led();
			}
		}
		if (( wakeup & WAKEUP_FROM_ADC1_IRQ) == WAKEUP_FROM_ADC1_IRQ)
		{
		}

	}
}
#endif // #ifdef SAMPLEPROCESS_1_ADC
#endif // #ifdef SAMPLE_PROCESSES_ENABLED
